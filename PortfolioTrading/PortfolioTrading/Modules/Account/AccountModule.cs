using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.ServiceLocation;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Utils;
using PortfolioTrading.Modules.Portfolio;

namespace PortfolioTrading.Modules.Account
{
    [ModuleExport(typeof(AccountModule))]
    public class AccountModule : IModule
    {
        private IRegionManager RegionManager { get; set; }
        private IEventAggregator EventAgg { get; set; }

        [ImportingConstructor]
        public AccountModule(IRegionManager regMgr, IEventAggregator evtAgg)
        {
            RegionManager = regMgr;
            EventAgg = evtAgg;
        }

        public void Initialize()
        {
            LogManager.Logger.Info("Account module initializing...");
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.AccountMgrRegion,
                () => ServiceLocator.Current.GetInstance<AccountMgrView>());
            regionMgr.RegisterViewWithRegion(RegionNames.AccountInfoRegion,
                () => ServiceLocator.Current.GetInstance<AccountInfoView>());

            EventAgg.GetEvent<QueryPositionEvent>().Subscribe(OnQueryPosition);

            // load server address list
            var serverAddrRepoVm = ServiceLocator.Current.GetInstance<ServerAddressRepoVM>();
            serverAddrRepoVm.LoadServerList();
        }

        private void OnQueryPosition(AccountVM acctVm)
        {
            string viewName = acctVm.InvestorId;
            var existingPane = RegionManager.GetExistingPane<PositionContentPane>(viewName);
            if (existingPane != null)
            {
                existingPane.Activate();
                PositionView positionView = existingPane.Content as PositionView;
                if (positionView != null)
                    positionView.ViewModel.RefreshPositionCommand.Execute();
            }
            else
            {
                PositionContentPane pane = new PositionContentPane();
                pane.ViewName = viewName;
                var positionView = ServiceLocator.Current.GetInstance<PositionView>();
                positionView.ViewModel.SetAccount(acctVm);
                positionView.ViewModel.RefreshPositionCommand.Execute();
                pane.Content = positionView;
                //pane.CloseAction = Infragistics.Windows.DockManager.PaneCloseAction.RemovePane;
                pane.Closed += new EventHandler<Infragistics.Windows.DockManager.Events.PaneClosedEventArgs>(pane_Closed);
                
                RegionManager.RegisterViewWithRegion(RegionNames.PortfolioViewRegion,
                                                    () => pane);
                pane.Activate();
            }
        }

        void pane_Closed(object sender, Infragistics.Windows.DockManager.Events.PaneClosedEventArgs e)
        {
            PositionContentPane pane = sender as PositionContentPane;
            if (pane != null)
            {
                PositionView positionView = pane.Content as PositionView;
                if (positionView != null)
                {
                    positionView.ViewModel.SetAccount(null);
                }
            }
            RegionManager.RemovePane(sender);
        }
    }
}

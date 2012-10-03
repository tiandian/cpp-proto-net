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
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Modules.Portfolio
{
    [ModuleExport(typeof(PortfolioModule))]
    public class PortfolioModule : IModule
    {
        private IRegionManager RegionManager { get; set; }
        private IEventAggregator EventAgg { get; set; }

        [ImportingConstructor]
        public PortfolioModule(IRegionManager regMgr, IEventAggregator evtAgg)
        {
            RegionManager = regMgr;
            EventAgg = evtAgg;
        }

        public void Initialize()
        {
            LogManager.Logger.Info("Portfolio module initializing...");
           
            RegionManager.RegisterViewWithRegion(RegionNames.OrdersRegion,
                () => ServiceLocator.Current.GetInstance<PortfolioOrdersView>());
            RegionManager.RegisterViewWithRegion(RegionNames.PortfolioSettingsRegion,
                () => ServiceLocator.Current.GetInstance<PortfolioSettingsView>());

            EventAgg.GetEvent<AccountSelectedEvent>().Subscribe(OnAccountSelected, ThreadOption.UIThread);
        }

        public void OnAccountSelected(AccountVM accountVm)
        {
            string viewName = accountVm.InvestorId;
            var existingPane = GetExistingPane(viewName);
            if (existingPane != null)
            {
                existingPane.Activate();
            }
            else
            {
                PortfolioContentPane pane = new PortfolioContentPane();
                pane.ViewName = viewName;
                var portfoliosView = ServiceLocator.Current.GetInstance<PortfoliosView>();
                portfoliosView.SetAccount(accountVm);
                pane.Content = portfoliosView;
                //pane.CloseAction = Infragistics.Windows.DockManager.PaneCloseAction.RemovePane;
                pane.Closed += new EventHandler<Infragistics.Windows.DockManager.Events.PaneClosedEventArgs>(pane_Closed);
                pane.Loaded += (s, e) => portfoliosView.OnViewSelected();
                
                RegionManager.RegisterViewWithRegion(RegionNames.PortfolioViewRegion,
                                                    () => pane);
                pane.Activate();
            }
        }

        void pane_Closed(object sender, Infragistics.Windows.DockManager.Events.PaneClosedEventArgs e)
        {
            RemovePane(sender);
        }

        private PortfolioContentPane GetExistingPane(string viewName)
        {
            if (RegionManager.Regions.ContainsRegionWithName(RegionNames.PortfolioViewRegion))
            {
                IRegion portfViewRegion = RegionManager.Regions[RegionNames.PortfolioViewRegion];
                foreach (var view in portfViewRegion.Views)
                {
                    PortfolioContentPane pane = view as PortfolioContentPane;
                    if (pane != null && pane.ViewName == viewName)
                        return pane;
                }
            }
            return null;
        }

        private void RemovePane(object pane)
        {
            if (RegionManager.Regions.ContainsRegionWithName(RegionNames.PortfolioViewRegion))
            {
                IRegion portfViewRegion = RegionManager.Regions[RegionNames.PortfolioViewRegion];
                portfViewRegion.Remove(pane);
            }
        }
    }
}

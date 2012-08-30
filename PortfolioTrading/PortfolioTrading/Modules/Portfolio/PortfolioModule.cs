using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.ServiceLocation;

namespace PortfolioTrading.Modules.Portfolio
{
    [ModuleExport(typeof(PortfolioModule))]
    public class PortfolioModule : IModule
    {
        public void Initialize()
        {
            LogManager.Logger.Info("Portfolio module initializing...");
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.PortfolioViewRegion,
                () => ServiceLocator.Current.GetInstance<PortfoliosView>());
            regionMgr.RegisterViewWithRegion(RegionNames.OrdersRegion,
                () => ServiceLocator.Current.GetInstance<PortfolioOrdersView>());
            regionMgr.RegisterViewWithRegion(RegionNames.PortfolioSettingsRegion,
                () => ServiceLocator.Current.GetInstance<PortfolioSettingsView>());

        }
    }
}

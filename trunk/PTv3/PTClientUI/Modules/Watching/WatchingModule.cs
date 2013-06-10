using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.ServiceLocation;

namespace PortfolioTrading.Modules.Watching
{
    [ModuleExport(typeof(WatchingModule))]
    public class WatchingModule : IModule
    {

        public void Initialize()
        {
            LogManager.Logger.Info("Watching module initializing...");
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.TimeNSalesRegion,
                () => ServiceLocator.Current.GetInstance<TimeNSalesView>());
            regionMgr.RegisterViewWithRegion(RegionNames.OperationsRegion,
                () => ServiceLocator.Current.GetInstance<OutputView>());
        }
    }
}

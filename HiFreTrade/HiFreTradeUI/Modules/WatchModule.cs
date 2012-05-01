using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.ServiceLocation;
using Microsoft.Practices.Prism.Regions;

namespace HiFreTradeUI.Modules
{
    [ModuleExport(typeof(WatchModule))]
    public class WatchModule : IModule
    {
        public void Initialize()
        {
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.OperationsRegion,
                () => ServiceLocator.Current.GetInstance<OperationsView>());
            regionMgr.RegisterViewWithRegion(RegionNames.TimeNSalesRegion,
                () => ServiceLocator.Current.GetInstance<TimeNSalesView>());
            regionMgr.RegisterViewWithRegion(RegionNames.PositionRegion,
                () => ServiceLocator.Current.GetInstance<PositionView>());
        }
    }
}

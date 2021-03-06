﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.ServiceLocation;

namespace HiFreTradeUI.Modules.PositionHandling
{
    [ModuleExport(typeof(PositionModule))]
    public class PositionModule : IModule
    {
        public void Initialize()
        {
            LogManager.Logger.Info("Position module initializing...");
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.PositionRegion,
                () => ServiceLocator.Current.GetInstance<PositionView>());
            regionMgr.RegisterViewWithRegion(RegionNames.OrdersRegion,
                () => ServiceLocator.Current.GetInstance<OrdersView>());
            
        }
    }
}

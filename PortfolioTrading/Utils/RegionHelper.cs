using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PortfolioTrading.Modules.Portfolio;
using Microsoft.Practices.Prism.Regions;
using Infragistics.Windows.DockManager;

namespace PortfolioTrading.Utils
{
    static class RegionHelper
    {
        public static T GetExistingPane<T>(this IRegionManager RegionManager, string viewName)
            where T : MyContentPane
        {
            if (RegionManager.Regions.ContainsRegionWithName(RegionNames.PortfolioViewRegion))
            {
                IRegion portfViewRegion = RegionManager.Regions[RegionNames.PortfolioViewRegion];
                foreach (var view in portfViewRegion.Views)
                {
                    T pane = view as T;
                    if (pane != null && pane.ViewName == viewName)
                        return pane;
                }
            }
            return null;
        }

        public static void RemovePane(this IRegionManager RegionManager, object pane)
        {
            if (RegionManager.Regions.ContainsRegionWithName(RegionNames.PortfolioViewRegion))
            {
                IRegion portfViewRegion = RegionManager.Regions[RegionNames.PortfolioViewRegion];
                portfViewRegion.Remove(pane);
            }
        }
    }
}

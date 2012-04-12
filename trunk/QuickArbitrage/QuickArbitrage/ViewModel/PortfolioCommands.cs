using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace QuickArbitrage.Main.ViewModel
{
    public static class PortfolioCommands
    {
        public static RoutedCommand LegOpenPosition = new RoutedCommand("LegOpenPosition", typeof(PortfolioCommands));

        public static RoutedCommand PortfolioOpenPosition = new RoutedCommand("PortfolioOpenPosition", typeof(PortfolioCommands));
    }
}

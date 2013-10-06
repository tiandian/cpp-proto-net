using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Modules.Portfolio.Strategy;

namespace PortfolioTrading.Controls
{
    public class PortfWatchTemplateSelector : DataTemplateSelector
    {
        public DataTemplate ArbitrageTemplate { get; set; }
        public DataTemplate MACDHistSlopeTemplate { get; set; }
        public DataTemplate MACDCrossTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item != null)
            {
                PortfolioVM portfVm = item as PortfolioVM;

                if (portfVm.StrategySetting.Name == StrategySetting.ArbitrageStrategyName)
                    return ArbitrageTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.MACDHistSlopeStrategyName)
                    return MACDHistSlopeTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.MACDCrossStrategyName)
                    return MACDCrossTemplate;
                else
                    return ArbitrageTemplate;
            }
            
            return null;
        }
    }
}

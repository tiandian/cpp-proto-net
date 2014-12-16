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
    public class PortfMonTemplateSelector : DataTemplateSelector
    {
        public DataTemplate ArbitrageTemplate { get; set; }
        public DataTemplate ScalperTemplate { get; set; }
        public DataTemplate ChangePositionTemplate { get; set; }
        public DataTemplate MACDHistSlopeTemplate { get; set; }
        public DataTemplate WMATrendTemplate { get; set; }
        public DataTemplate LinerRegressionTemplate { get; set; }
        public DataTemplate ASCTrendTemplate { get; set; }
        public DataTemplate RangeTrendTemplate { get; set; }
        public DataTemplate ManualTemplate { get; set; }
        public DataTemplate QuickScoreTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            PortfolioVM portfVm = item as PortfolioVM;
            if (portfVm != null)
            {
                if (portfVm.StrategySetting.Name == StrategySetting.ArbitrageStrategyName)
                    return ArbitrageTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.ChangePositionStrategyName)
                    return ChangePositionTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.ScalperStrategyName)
                    return ScalperTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.MACDHistSlopeStrategyName)
                    return MACDHistSlopeTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.WMATrendStrategyName)
                    return WMATrendTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.LinerRegressionStrategyName)
                    return LinerRegressionTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.ASCTrendStrategyName)
                    return ASCTrendTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.RangeTrendStrategyName)
                    return RangeTrendTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.ManualStrategyName)
                    return ManualTemplate;
                else if (portfVm.StrategySetting.Name == StrategySetting.QuickScoreStrategyName)
                    return QuickScoreTemplate;
            }

            return null;
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Controls
{
    public class PortfMonTemplateSelector : DataTemplateSelector
    {
        public DataTemplate ArbitrageTemplate { get; set; }
        public DataTemplate BoxBreakTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            PortfolioVM portfVm = item as PortfolioVM;
            if (portfVm != null)
            {
                return ArbitrageTemplate;
            }

            return null;
        }

    }
}

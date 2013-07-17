using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using PortfolioTrading.Modules.Portfolio.Strategy;
using PTEntity;

namespace PortfolioTrading.Converters
{
    [ValueConversion(typeof(CompareCondition), typeof(string))]
    public class CompareConditionDisplayConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            CompareCondition cond = (CompareCondition)value;
            switch (cond)
            {
                case CompareCondition.GREATER_THAN:
                    return "大于";
                case CompareCondition.GREATER_EQUAL_THAN:
                    return "大于等于";
                case CompareCondition.LESS_THAN:
                    return "小于";
                case CompareCondition.LESS_EQUAL_THAN:
                    return "小于等于";
                default:
                    return "未知";
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}

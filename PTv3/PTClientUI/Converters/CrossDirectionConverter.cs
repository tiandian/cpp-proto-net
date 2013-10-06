using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace PortfolioTrading.Converters
{
    [ValueConversion(typeof(double), typeof(string))]
    public class CrossDirectionConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double val = (double)value;
            if (val > 0)
                return "金叉";
            if (val < 0)
                return "死叉";

            return "未知";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}

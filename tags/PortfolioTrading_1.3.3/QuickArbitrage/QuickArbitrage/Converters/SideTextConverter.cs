using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.Converters
{
    [ValueConversion(typeof(Side), typeof(string))]
    class SideTextConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string ret = "未知";
            Side side = (Side)value;
            switch (side)
            {
                case Side.Long:
                    ret = "多头";
                    break;
                case Side.Short:
                    ret = "空头";
                    break;
                default:
                    break;
            }
            return ret;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}

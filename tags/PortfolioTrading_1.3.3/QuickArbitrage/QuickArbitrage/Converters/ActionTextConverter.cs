using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.Converters
{
    [ValueConversion(typeof(TradeAction), typeof(string))]
    class ActionTextConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string ret = "未知";
            TradeAction action = (TradeAction)value;
            switch (action)
            {
                case TradeAction.LongEntry:
                    ret = "多开";
                    break;
                case TradeAction.LongExit:
                    ret = "多平";
                    break;
                case TradeAction.ShortEntry:
                    ret = "空开";
                    break;
                case TradeAction.ShortExit:
                    ret = "空平";
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.Converters
{
    [ValueConversion(typeof(TradingType), typeof(string))]
    class TradingTypeTextConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string ret = "未知";
            TradingType tradingType = (TradingType)value;
            switch (tradingType)
            {
                case TradingType.Manual:
                    ret = "手动";
                    break;
                case TradingType.Auto:
                    ret = "自动";
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

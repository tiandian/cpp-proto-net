using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace PortfolioTrading.Converters
{
    [ValueConversion(typeof(PTEntity.SlopeDirection), typeof(string))]
    public class SlopeDirectionConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            PTEntity.SlopeDirection direction = (PTEntity.SlopeDirection)value;
            switch (direction)
            {
                case PTEntity.SlopeDirection.GOING_UP:
                    return "向上";
                case PTEntity.SlopeDirection.GOING_DOWN:
                    return "向下";
                case PTEntity.SlopeDirection.NO_DIRECTION:
                    return "走平";
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

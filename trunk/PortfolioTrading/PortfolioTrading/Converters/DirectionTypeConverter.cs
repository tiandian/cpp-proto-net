using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace PortfolioTrading.Converters
{
    [ValueConversion(typeof(entity.PosiDirectionType), typeof(string))]
    public class DirectionTypeConverter : IValueConverter
    {
        public System.Object Convert(System.Object value, System.Type targetType, System.Object parameter, System.Globalization.CultureInfo culture)
        {
            entity.PosiDirectionType direction = (entity.PosiDirectionType)value;
            switch (direction)
            {
                case entity.PosiDirectionType.LONG:
                    return "多头";
                case entity.PosiDirectionType.SHORT:
                    return "空头";
                default:
                    return "未知";
            }
        }

        public System.Object ConvertBack(System.Object value, System.Type targetType, System.Object parameter, System.Globalization.CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }
}

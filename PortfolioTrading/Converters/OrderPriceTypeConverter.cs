using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace PortfolioTrading.Converters
{
    public class OrderPriceTypeConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (values.Length > 1)
            {
                string priceType = values[0].ToString();
                double limitPrice = System.Convert.ToDouble(values[1]);
                string priceTxt = limitPrice > 0 ? 
                    string.Format("{0} {1}", priceType, limitPrice) : 
                    priceType;

                return priceTxt;
            }

            return null;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}

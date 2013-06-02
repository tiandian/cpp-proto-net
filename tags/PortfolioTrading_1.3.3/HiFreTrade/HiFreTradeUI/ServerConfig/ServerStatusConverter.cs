using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace HiFreTradeUI.ServerConfig
{
    public class ServerStatusConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            ServerStatus status = (ServerStatus)value;
            switch (status)
            {
                case ServerStatus.Testing:
                    return "测试中";
                case ServerStatus.Available:
                    return "连接正常";
                case ServerStatus.Failed:
                    return "无法连接";
                case ServerStatus.Unknown:
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

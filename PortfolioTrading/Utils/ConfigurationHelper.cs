using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace PortfolioTrading.Utils
{
    static class ConfigurationHelper
    {
        public static T GetAppSettingValue<T>(string key, T defaultValue)
        {
            string val = ConfigurationManager.AppSettings[key];
            if (!string.IsNullOrEmpty(val))
            {
                try
                {
                    defaultValue = (T)Convert.ChangeType(val, typeof(T));
                }
                catch
                {
                }
            }
            return defaultValue;
        }
    }
}

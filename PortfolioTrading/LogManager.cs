using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PortfolioTrading.Utils;

namespace PortfolioTrading
{
    public static class LogManager
    {
        static LogManager()
        {
            log4net.Config.XmlConfigurator.Configure();

            _logger = new Log4netLogger(log4net.LogManager.GetLogger("Default"));
        }

        private static ILog _logger = null;

        public static ILog Logger
        {
            get { return _logger; }
        }

        public static ILog GetLogger(Type type)
        {
            return new Log4netLogger(log4net.LogManager.GetLogger(type));
        }
    }
}

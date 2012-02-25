using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common.Contract;
using QuickArbitrage.Common.Logging;

namespace QuickArbitrage.Common
{
    public static class LogManager
    {
        static LogManager()
        {
            log4net.Config.XmlConfigurator.Configure();

            _logger = new MyLogger(log4net.LogManager.GetLogger("Default"));
        }

        private static ILog _logger = null;

        public static ILog Logger
        {
            get { return _logger; }
        }

        public static ILog GetLogger(Type type)
        {
            return new MyLogger(log4net.LogManager.GetLogger(type));
        }
    }

}

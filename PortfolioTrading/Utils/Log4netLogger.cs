using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Logging;

namespace PortfolioTrading.Utils
{
    class Log4netLogger : ILog, ILoggerFacade
    {
        private log4net.ILog _log4netLogger;

        public Log4netLogger(log4net.ILog log4netLogger)
        {
            _log4netLogger = log4netLogger;
        }

        public void Debug(object message)
        {
            _log4netLogger.Debug(message);
        }

        public void DebugFormat(string format, params object[] args)
        {
            _log4netLogger.DebugFormat(format, args);
        }

        public void Error(object message)
        {
            _log4netLogger.Error(message);
        }

        public void ErrorFormat(string format, params object[] args)
        {
            _log4netLogger.ErrorFormat(format, args);
        }

        public void Fatal(object message)
        {
            _log4netLogger.Fatal(message);
        }

        public void FatalFormat(string format, params object[] args)
        {
            _log4netLogger.FatalFormat(format, args);
        }

        public void Info(object message)
        {
            _log4netLogger.Info(message);
        }

        public void InfoFormat(string format, params object[] args)
        {
            _log4netLogger.InfoFormat(format, args);
        }

        public void Warn(object message)
        {
            _log4netLogger.Warn(message);
        }

        public void WarnFormat(string format, params object[] args)
        {
            _log4netLogger.WarnFormat(format, args);
        }

        public void Log(string message, Category category, Priority priority)
        {
            switch (category)
            {
                case Category.Debug:
                    Debug(message);
                    break;
                case Category.Exception:
                    Error(message);
                    break;
                case Category.Info:
                    Info(message);
                    break;
                case Category.Warn:
                    Warn(message);
                    break;
                default:
                    Debug(message);
                    break;
            }
        }
    }
}

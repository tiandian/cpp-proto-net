using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common.Contract;

namespace QuickArbitrage.Common.Logging
{
    class MyLogger : ILog
    {
        private log4net.ILog _log4netLogger;

        public MyLogger(log4net.ILog log4netLogger)
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
    }
}

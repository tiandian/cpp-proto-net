using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.ServiceLocation;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;

namespace PortfolioTrading.Utils
{
    public static class EventLogger
    {
        private static IEventAggregator EventAggregator { get; set; }
        static EventLogger()
        {
            EventAggregator = ServiceLocator.Current.GetInstance<IEventAggregator>();
        }

        public static void Write(string message)
        {
            EventAggregator.GetEvent<OutputEvent>().Publish(message);
        }

        public static void Write(string format, params object[] args)
        {
            Write(string.Format(format, args));
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using System.Windows.Threading;
using System.Threading;

namespace PortfolioTrading.Utils
{
    public static class DispatcherHelper
    {
        private static Dispatcher _dispatcher;

        public static void Init(Dispatcher disp)
        {
            _dispatcher = disp;
        }

        public static Dispatcher Current
        {
            get { return _dispatcher; }
        }
    }
}

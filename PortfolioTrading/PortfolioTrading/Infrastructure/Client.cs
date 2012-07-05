using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using APInvokeManaged;

namespace PortfolioTrading.Infrastructure
{
    class Client : ClientBase
    {
        public bool QuoteConnect(string quoteAddress, string streamDir)
        {
            return true;
        }

        protected override void DispatchCallback(string method, byte[] paramData)
        {
            throw new NotImplementedException();
        }
    }
}

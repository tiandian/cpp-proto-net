using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common.Contract;

namespace QuickArbitrage.Connection
{
    public class ClientFactory
    {
        private static ClientFactory factory = null;
        static ClientFactory()
        {
            factory = new ClientFactory();
        }
        public static ClientFactory Instance
        {
            get { return factory; }
        }

        private Client _client = null;

        private ClientFactory()
        {
            _client = new Client();
        }

        public IQuoteClient GetQuoteClient()
        {
            return _client;
        }

        public IAccountClient GetAccountClient()
        {
            return _client;
        }

        public ITradeClient GetTradeClient()
        {
            return _client;
        }
    }
}

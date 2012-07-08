using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using APInvokeManaged;
using entity;

namespace PortfolioTrading.Infrastructure
{
    class Client : ClientBase
    {
        public OperationResult QuoteConnect(string quoteAddress, string streamDir)
        {
            return ServerConnect("QuoteConnect", quoteAddress, streamDir);
        }

        public void QuoteDisconnect()
        {
            ServerDisconnect("QuoteDisconnect");
        }

        public OperationResult TradeConnect(string tradeAddress, string streamDir)
        {
            return ServerConnect("TradeConnect", tradeAddress, streamDir);
        }

        public void TradeDisconnect()
        {
            ServerDisconnect("TradeDisconnect");
        }

        private OperationResult ServerConnect(string method, string servAddress, string streamDir)
        {
            ConnectParam connParam = new ConnectParam()
            {
                QuoteAddress = servAddress,
                StreamFolder = streamDir
            };

            byte[] param_data = DataTranslater.Serialize(connParam);

            byte[] ret_data = Request(method, param_data);
            if (ret_data != null)
            {
                OperationReturn rt = DataTranslater.Deserialize<OperationReturn>(ret_data);
                return new OperationResult
                {
                    Success = rt.Success,
                    ErrorMessage = rt.ErrorMessage
                };
            }
            else
                return new OperationResult
                {
                    Success = false,
                    ErrorMessage = "Returned data is invalid"
                };
        }

        private void ServerDisconnect(string method)
        {
            byte[] void_ret = Request(method, ClientBase.VoidParam);
        }

        protected override void DispatchCallback(string method, byte[] paramData)
        {
            throw new NotImplementedException();
        }
    }

    class OperationResult
    {
        public bool Success { get; set; }
        public string ErrorMessage { get; set; }
    }
}

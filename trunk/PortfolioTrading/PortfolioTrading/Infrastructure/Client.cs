using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using APInvokeManaged;
using entity;
using System.Diagnostics;
using trade;

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

        public OperationResult QuoteLogin(string brokerId, string userId, string password)
        {
            return ServerLogin("QuoteLogin", brokerId, userId, password);
        }

        public void QuoteLogout()
        {
            ServerLogout("QuoteLogout");
        }

        public OperationResult TradeLogin(string brokerId, string userId, string password)
        {
            return ServerLogin("TradeLogin", brokerId, userId, password);
        }

        public void TradeLogout()
        {
            ServerLogout("TradeLogout");
        }

        public void RegisterQuote(string[] symbols)
        {
            RegQuoteParam regQuoteParam = new RegQuoteParam();
            regQuoteParam.Symbols.AddRange(symbols);

            byte[] param_data = DataTranslater.Serialize(regQuoteParam);

            byte[] ret_data = Request("RegQuote", param_data);
        }

        public void AddPortf(PortfolioItem portfItem)
        {
            Debug.Assert(!string.IsNullOrEmpty(portfItem.ID), "Portfolio item must have ID already");
            if(string.IsNullOrEmpty(portfItem.ID)) return;

            byte[] portf_data = DataTranslater.Serialize(portfItem);

            byte[] ret_data = Request("AddPortf", portf_data);
        }

        public void RemovePortf(string portfId)
        {
            StringParam strParam = new StringParam() { Data = portfId };
            byte[] param_data = DataTranslater.Serialize(strParam);

            byte[] ret_data = Request("RemovePortf", param_data);
        }

        public void PorfOpenPosition(string pid, int quantity)
        {
            PorfOpenPosiParam opParam = new PorfOpenPosiParam();
            opParam.PortfId = pid;
            opParam.Quantity = quantity;

            byte[] param_data = DataTranslater.Serialize(opParam);
            byte[] ret_data = Request("PorfOpenPosition", param_data);
        }

        public void PortfClosePosition(string mlOrderId, string legRef)
        {
            ClosePositionParam cpParam = new ClosePositionParam();
            cpParam.MultiLegOrderId = mlOrderId;
            cpParam.LegOrdRef = legRef;

            byte[] param_data = DataTranslater.Serialize(cpParam);
            byte[] ret_data = Request("ClosePosition", param_data);
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
                    ErrorMessage = "Returned data (Connect) is invalid"
                };
        }

        private void ServerDisconnect(string method)
        {
            byte[] void_ret = Request(method, ClientBase.VoidParam);
        }

        private OperationResult ServerLogin(string method, string brokerId, string userId, string password)
        {
            LoginParam connParam = new LoginParam()
            {
                BrokerId = brokerId,
                UserId = userId,
                Password = password
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
                    ErrorMessage = "Returned data (Login) is invalid"
                };
        }

        private void ServerLogout(string method)
        {
            byte[] void_ret = Request(method, ClientBase.VoidParam);
        }

        public event Action<Quote> OnQuoteReceived;
        public event Action<PortfolioItem> OnPortfolioItemUpdated;
        public event Action<MultiLegOrder> OnMultiLegOrderUpdated;
        public event Action<Trade> OnTradeUpdated;

        protected override void DispatchCallback(string method, byte[] paramData)
        {
            if (method == "QuotePush")
            {
                if (OnQuoteReceived != null)
                {
                    Quote quoteData = DataTranslater.Deserialize<Quote>(paramData);
                    OnQuoteReceived(quoteData);
                }
            }
            else if (method == "PortfolioPush")
            {
                if (OnPortfolioItemUpdated != null)
                {
                    PortfolioItem porfItem = DataTranslater.Deserialize<PortfolioItem>(paramData);
                    OnPortfolioItemUpdated(porfItem);
                }
            }
            else if (method == "MultiLegOrderPush")
            {
                if (OnMultiLegOrderUpdated != null)
                {
                    MultiLegOrder mlOrder = DataTranslater.Deserialize<MultiLegOrder>(paramData);
                    OnMultiLegOrderUpdated(mlOrder);
                }
            }
            else if (method == "TradePush")
            {
                if (OnTradeUpdated != null)
                {
                    Trade trade = DataTranslater.Deserialize<Trade>(paramData);
                    OnTradeUpdated(trade);
                }
            }
        }
    }

    class OperationResult
    {
        public bool Success { get; set; }
        public string ErrorMessage { get; set; }
    }
}

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

        public void AddPortfCollection(IEnumerable<PortfolioItem> portfItems)
        {
            AddPortfolioParam addPortfParam = new AddPortfolioParam();
            addPortfParam.PortfolioItems.AddRange(portfItems);

            byte[] portf_data = DataTranslater.Serialize(addPortfParam);

            byte[] ret_data = Request("AddPortfCollection", portf_data);
        }

        public void RemovePortf(string portfId)
        {
            StringParam strParam = new StringParam() { Data = portfId };
            byte[] param_data = DataTranslater.Serialize(strParam);

            byte[] ret_data = Request("RemovePortf", param_data);
        }

        public void PorfOpenPosition(string pid, int quantity, bool isVirtual)
        {
            PorfOpenPosiParam opParam = new PorfOpenPosiParam();
            opParam.PortfId = pid;
            opParam.Quantity = quantity;
            opParam.IsVirtual = isVirtual;

            byte[] param_data = DataTranslater.Serialize(opParam);
            byte[] ret_data = Request("PorfOpenPosition", param_data);
        }

        public void PorfClosePosition(string pid, int quantity, bool isVirtual)
        {
            PorfOpenPosiParam opParam = new PorfOpenPosiParam();
            opParam.PortfId = pid;
            opParam.Quantity = quantity;
            opParam.IsVirtual = isVirtual;

            byte[] param_data = DataTranslater.Serialize(opParam);
            byte[] ret_data = Request("PorfClosePosition", param_data);
        }

        public string PortfClosePosition(trade.MultiLegOrder mlOrder, string legRef)
        {
            ClosePositionParam cpParam = new ClosePositionParam();
            cpParam.MultiLegOrder = mlOrder;
            cpParam.LegOrdRef = legRef;

            byte[] param_data = DataTranslater.Serialize(cpParam);
            byte[] ret_data = Request("ClosePosition", param_data);

            StringParam strReturn = DataTranslater.Deserialize<StringParam>(ret_data);
            return strReturn.Data;
        }

        public void CancelOrder(trade.Order legOrder)
        {
            CancelOrderParam cxlParam = new CancelOrderParam();
            cxlParam.OrderRef = legOrder.OrderRef;
            cxlParam.ExchangeId = legOrder.ExchangeID;
            cxlParam.OrdSysId = legOrder.OrderSysID;
            cxlParam.UserId = legOrder.UserID;
            cxlParam.Symbol = legOrder.InstrumentID;

            byte[] param_data = DataTranslater.Serialize(cxlParam);
            byte[] ret_data = Request("CancelOrder", param_data);
        }

        public void PortfModifyQuantity(string portfId, int onceQty, int maxQty)
        {
            ModifyPortfolioQtyParam qtyParam = new ModifyPortfolioQtyParam();
            qtyParam.PortfId = portfId;
            qtyParam.OnceQty = onceQty;
            qtyParam.MaxQty = maxQty;

            byte[] param_data = DataTranslater.Serialize(qtyParam);
            byte[] ret_data = Request("PortfChgQuantity", param_data);
        }

        public void PortfSetPreferredLeg(string portfId, string legName)
        {
            entity.ModifyPortfolioPreferredLegParam modifyParam = new entity.ModifyPortfolioPreferredLegParam();
            modifyParam.PortfId = portfId;
            modifyParam.LegSymbol = legName;

            byte[] param_data = DataTranslater.Serialize(modifyParam);
            byte[] ret_data = Request("PortfSetPreferredLeg", param_data);
        }

        public void PortfEnableStrategy(string portfId, bool isEnabled)
        {
            entity.ModifyRunningStatusParam runningParam = new entity.ModifyRunningStatusParam();
            runningParam.PortfId = portfId;
            runningParam.Enabled = isEnabled;

            byte[] param_data = DataTranslater.Serialize(runningParam);
            byte[] ret_data = Request("PortfEnableStrategy", param_data);
        }

        public void PortfTurnSwitches(string portfId, bool autoOpen, bool autoStopGain, bool autoStopLoss,
            bool autoTracking, bool enablePrefer)
        {
            entity.ModifyPortfolioSwitchParam modifyParam = new entity.ModifyPortfolioSwitchParam();
            modifyParam.PortfId = portfId;
            modifyParam.AutoOpen = autoOpen;
            modifyParam.AutoStopGain = autoStopGain;
            modifyParam.AutoStopLoss = autoStopLoss;
            modifyParam.AutoTracking = autoTracking;
            modifyParam.EnablePrefer = enablePrefer;

            byte[] param_data = DataTranslater.Serialize(modifyParam);
            byte[] ret_data = Request("PortfTurnSwitches", param_data);
        }

        public void PortfApplyStrategySettings(string portfId, string strategyName, byte[] strategyData)
        {
            entity.ModifyStrategyParam modifyParam = new entity.ModifyStrategyParam();
            modifyParam.PortfId = portfId;
            modifyParam.StrategyName = strategyName;
            modifyParam.StrategyData = strategyData;
            byte[] param_data = DataTranslater.Serialize(modifyParam);
            byte[] ret_data = Request("ApplyStrategySettings", param_data);
        }

        //public void PortfSetPreferLeg(string portfId, string legId, )

        public trade.AccountInfo QueryAccountInfo()
        {
            byte[] ret_data = Request("QueryAccountInfo", new byte[]{});

            return DataTranslater.Deserialize<trade.AccountInfo>(ret_data);
        }

        public void QueryPositionDetails(string symbol)
        {
            entity.StringParam strParam = new entity.StringParam();
            strParam.Data = symbol;
            byte[] param_data = DataTranslater.Serialize(strParam);
            byte[] ret_data = Request("QueryPositionDetails", param_data);
        }

        public OperationResult ManualCloseOrder(string symbol, trade.TradeDirectionType direction,
            trade.OffsetFlagType offsetFlag, int quantity)
        {
            entity.ManualCloseOrderParam closeOrdParam = new entity.ManualCloseOrderParam();
            closeOrdParam.Symbol = symbol;
            closeOrdParam.Direction = direction;
            closeOrdParam.OffsetFlag = offsetFlag;
            closeOrdParam.Quantity = quantity;

            byte[] param_data = DataTranslater.Serialize(closeOrdParam);
            byte[] ret_data = Request("ManualCloseOrder", param_data);

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
                    ErrorMessage = "Returned data (ManualCloseOrder) is invalid"
                };
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
            byte[] void_ret = Request(method, ClientBase.VoidParam, 5000);
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
            RequestAsync(method, ClientBase.VoidParam, null);
        }

        public event Action<Quote> OnQuoteReceived;
        public event Action<PortfolioItem> OnPortfolioItemUpdated;
        public event Action<MultiLegOrder> OnMultiLegOrderUpdated;
        public event Action<string, string, string, Order> OnLegOrderUpdated;
        public event Action<Trade> OnTradeUpdated;
        public event Action<PositionDetailInfo> OnPositionDetialReturn;

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
            else if (method == "LegOrderPush")
            {
                if(OnLegOrderUpdated != null)
                {
                    LegOrderUpdateParam orderUpdateParam = DataTranslater.Deserialize<LegOrderUpdateParam>(paramData);
                    OnLegOrderUpdated(orderUpdateParam.PortfId,
                                      orderUpdateParam.MultiLegOrderId, 
                                      orderUpdateParam.LegOrderRef,
                                      orderUpdateParam.LegOrder);
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
            else if (method == "PositionDetailReturn")
            {
                if (OnPositionDetialReturn != null)
                {
                    PositionDetailInfo posiDetail = DataTranslater.Deserialize<PositionDetailInfo>(paramData);
                    OnPositionDetialReturn(posiDetail);
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

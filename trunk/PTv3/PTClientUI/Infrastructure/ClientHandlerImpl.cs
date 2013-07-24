using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PTCommunication;

namespace PortfolioTrading.Infrastructure
{
    class ClientHandlerImpl : IClientRequestHandler
    {
        public event Action<bool, string> OnLogin;
        public event Action<PTEntity.ServerType, bool, string> OnServerLogin;
        public event Action<PTEntity.PortfolioUpdateItem> OnPortfolioUpdated;
        public event Action<PTEntity.MultiLegOrder> OnMultiLegOrderUpdated;
        public event Action<PTEntity.TradeUpdate> OnTradeUpdated;
        public event Action<string, string, string, PTEntity.Order> OnLegOrderUpdated;
        public event Action OnConnectionClosed;

        public void OnLoginReturned(PTEntity.LoginReturn loginReturn)
        {
            LogManager.Logger.DebugFormat("Login result: {0}, {1}", loginReturn.Success, loginReturn.ErrorMessage);
            if (OnLogin != null)
                OnLogin(loginReturn.Success, loginReturn.ErrorMessage);
        }

        public void OnServerLoginReturned(PTEntity.ServerLoginReturn loginReturn)
        {
            LogManager.Logger.DebugFormat("Server login result: {0}, {1}", loginReturn.Success, loginReturn.ErrorMessage);
            if (OnServerLogin != null)
                OnServerLogin(loginReturn.SeverType, loginReturn.Success, loginReturn.ErrorMessage);
        }

        public void OnPortfolioUpdate(PTEntity.PortfolioUpdateItem portfolioUpdateItem)
        {
            if (OnPortfolioUpdated != null)
                OnPortfolioUpdated(portfolioUpdateItem);
        }

        public void OnLegOrderUpdate(string portfId, string mlOrderId, string legOrdRef, PTEntity.Order legOrder)
        {
            if (OnLegOrderUpdated != null)
                OnLegOrderUpdated(portfId, mlOrderId, legOrdRef, legOrder);
        }

        public void OnMultiLegOrderUpdate(PTEntity.MultiLegOrder multiLegOrder)
        {
            if (OnMultiLegOrderUpdated != null)
                OnMultiLegOrderUpdated(multiLegOrder);
        }

        public void OnTradeUpdate(PTEntity.TradeUpdate trade)
        {
            if (OnTradeUpdated != null)
                OnTradeUpdated(trade);
        }

        public void OnDisconnected()
        {
            if (OnConnectionClosed != null)
                OnConnectionClosed();
        }

        public void OnHeartbeat(string timestamp)
        {
            System.Diagnostics.Debug.WriteLine(string.Format("Server heartbeat received: {0}", timestamp));
        }
    }
}

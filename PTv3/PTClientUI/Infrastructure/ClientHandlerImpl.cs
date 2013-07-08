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
    }
}

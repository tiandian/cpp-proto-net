using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PTCommunication;

namespace PortfolioTrading.Infrastructure
{
    class ClientHandlerImpl : IClientRequestHandler
    {
        public void OnLoginReturned(PTEntity.LoginReturn loginReturn)
        {
            System.Console.WriteLine(string.Format("Login result: {0}, {1}", loginReturn.Success, loginReturn.ErrorMessage));
        }

        public void OnServerLoginReturned(PTEntity.ServerLoginReturn loginReturn)
        {
            System.Console.WriteLine(string.Format("Server login result: {0}, {1}", loginReturn.Success, loginReturn.ErrorMessage));
        }
    }
}

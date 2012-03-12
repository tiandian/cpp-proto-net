using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Common.Contract
{
    public delegate void LoginCallback(bool succ, string msg);

    public interface IAccountClient
    {
        void Login(string username, string password, LoginCallback callback);

        void Logout();
    }
}

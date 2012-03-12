using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common.Contract;
using QuickArbitrage.Connection.DataEntity;
using QuickArbitrage.Common;


namespace QuickArbitrage.Connection
{
    partial class Client : ConnectionBase, IAccountClient
    {
        private LoginCallback _loginResultCallback;
         
        public void Login(string username, string password, LoginCallback callback)
        {
            _loginResultCallback = callback;

            ConnectDoneCallback connectComplete = delegate(bool succ, string msg)
                {
                    if (succ)
                    {
                        // keep async reading
                        StartReading();

                        // REAL log in
                        ReqLogin req = new ReqLogin();
                        req.client = "Quick Arbitrage client";
                        req.ip = "127.0.0.1";
                        req.username = username;
                        req.password = password;

                        RequestLogin(req);
                    }
                    else
                    {
                        callback(false, msg);
                    }
                };

            ConnectAsync(new ConnectDoneCallback(connectComplete));
        }

        private void RequestLogin(ReqLogin request)
        {
            byte[] data = Serialize(request);

            SendAsync(MsgType.REQ_LOGIN, data, new SendDoneCallback(
                (succ, msg) => {
                    if (succ)
                    {
                        LogManager.Logger.Debug("Request login sent.");
                    }
                    else
                    {
                        LogManager.Logger.WarnFormat("Request login sending failed due to {0}", msg);
                    }
                }));
        }

        private void OnLoginResponse(RspLogin response)
        {
            if (_loginResultCallback != null)
                _loginResultCallback(response.succ, response.session);
        }

        public void Logout()
        {
            throw new NotImplementedException();
        }
    }
}

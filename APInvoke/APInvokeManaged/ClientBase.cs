using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APInvokeManaged
{
    public class ClientBase
    {
        private static string IdentifyCode = "xixihaha";

        private ConnectionBase _connection;

        public ClientBase()
        {
            _connection = new ConnectionBase();
        }

        public void ConnectAsync(string address, int port, Action<bool, string> connectCompletionHandler)
        {
            _connection.Address = address;
            _connection.Port = port;

            _connection.OpenAsync(
                delegate(bool succ, string err)
                {
                    if (connectCompletionHandler != null)
                        connectCompletionHandler(succ, err);
                });
        }


    }
}

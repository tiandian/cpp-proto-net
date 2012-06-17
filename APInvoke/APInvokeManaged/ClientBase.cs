using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;

namespace APInvokeManaged
{
    public abstract class ClientBase
    {
        private static string _authId = "APInvoke Connection";
        private static string _authPwd = "";

        public static string AuthId
        {
            get { return _authId; }
            set { _authId = value; }
        }
        public static string AuthPwd
        {
            get { return _authPwd; }
            set { _authPwd = value; }
        }

        private ConnectionBase _connection;
        private Dictionary<string, Delegate> _requestDic = new Dictionary<string, Delegate>();
        private object _syncObj = new object();

        public event Action<string> OnError;
        public bool IsConnected { get; private set; }
        public string SessionId { get; private set; }

        public ClientBase()
        {
            _connection = new ConnectionBase();
            _connection.OnDataReceived += new Action<MsgType, byte[]>(_connection_OnDataReceived);
            _connection.OnError += new Action<string>(_connection_OnError);
        }

        public bool Connect(string address, int port)
        {
            lock(_syncObj)
            {
                bool connected = false;
                ConnectAsync(address, port, 
                    delegate(bool succ, string err)
                    {
                        lock (_syncObj)
                        {
                            connected = succ;
                            Monitor.Pulse(_syncObj);
                        }
                    });

                Monitor.Wait(_syncObj);
                return connected;
            }
        }

        public void ConnectAsync(string address, int port, Action<bool, string> connectCompletionHandler)
        {
            _connection.Address = address;
            _connection.Port = port;

            _connection.OpenAsync(
                delegate(bool succ, string err)
                {
                    if (succ)
                    {
                        // keep async reading
                        _connection.StartReading();

                        EstablishConnection(connectCompletionHandler);
                    }
                    else
                    {
                        if (connectCompletionHandler != null)
                            connectCompletionHandler(succ, err);
                    }
                });
        }

        public void Disconnect()
        {
            _connection.Close();
        }

        protected abstract void DispatchMessage();

        void _connection_OnDataReceived(MsgType msgType, byte[] receivedData)
        {
            switch (msgType)
            {
                case MsgType.CONN_ACK:
                    OnEstablishConnectionAck(receivedData);
                    break;
                case MsgType.RSP:
                    break;
                case MsgType.CALLBK_REQ:
                    break;
                default:
                    RaiseError(string.Format("Unexpected message ({0}) received", msgType));
                    break;
            }
        }


        void _connection_OnError(string errMsg)
        {
            if (!_connection.IsConnected)
                IsConnected = false;

            RaiseError(errMsg);
        }

        private void RaiseError(string errMsg)
        {
            if (OnError != null)
                OnError(errMsg);
        }

        private void EstablishConnection(Action<bool, string> connectCompletionHandler)
        {
            Packet.Connect connReq = new Packet.Connect();
            connReq.userid = AuthId;
            connReq.password = AuthPwd;

            byte[] data = DataTranslater.Serialize(connReq);

            BookRequest(typeof(Packet.Connect).Name, connectCompletionHandler);

            _connection.SendAsync(MsgType.CONN, data, 
                delegate(bool succ, string msg)
                {
                    if (succ)
                    {
                        Trace.WriteLine("Request establish connection");
                    }
                    else
                    {
                        RemoveBookedRequest(typeof(Packet.Connect).Name);
                        connectCompletionHandler(
                            false, string.Format("Attempt to establish connection failed due to {0}", msg));
                    }
                });
        }

        private void OnEstablishConnectionAck(byte[] data)
        {
            Packet.ConnectAck connAck = DataTranslater.Deserialize<Packet.ConnectAck>(data);
            IsConnected = connAck.success;
            SessionId = connAck.session;

            Delegate callback = GetResponseCallback(typeof(Packet.Connect).Name);
            if (callback != null)
            {
                Action<bool, string> connCompletion = callback as Action<bool, string>;
                if (connCompletion != null)
                    connCompletion(IsConnected, "");
            }
        }

        private void BookRequest(string method, Delegate rspCallback)
        {
            lock (_requestDic)
            {
                if(_requestDic.ContainsKey(method))
                    throw new InvalidOperationException(
                        string.Format("Last invoke of '{0}' has not completed yet", method));

                _requestDic.Add(method, rspCallback);
            }
        }

        private Delegate GetResponseCallback(string method)
        {
            lock(_requestDic)
            {
                if (_requestDic.ContainsKey(method))
                {
                    Delegate callback = _requestDic[method];
                    _requestDic.Remove(method);
                    return callback;
                }

                RaiseError(string.Format("Unexpected method response ({0})", method));
                return null;
            }
        }

        private void RemoveBookedRequest(string method)
        {
            lock (_requestDic)
            {
                _requestDic.Remove(method);
            }
        }
    }
}

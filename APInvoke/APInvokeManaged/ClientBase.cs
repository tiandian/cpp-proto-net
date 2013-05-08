using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;

namespace APInvokeManaged
{
    public delegate void ResponseHandlerDel(bool invoke, byte[] ret_data, string errorMsg);

    public abstract class ClientBase
    {
        private static string _authId = "APInvoke Client";
        private static string _authPwd = "APInvoke Connection";

        public static readonly byte[] VoidParam = new byte[0];

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

        public TimeSpan SyncInvocationTimeout { get; set; } 

        private ConnectionBase _connection;
        private Dictionary<string, Delegate> _requestDic = new Dictionary<string, Delegate>();
        private object _syncObj = new object();

        public event Action<string> OnError;
        public bool IsConnected { get; private set; }
        public string SessionId { get; private set; }
        public bool AttachExisting { get; private set; }

        private string _authClientId;
        public string AuthClientId
        {
            get
            {
                if (string.IsNullOrEmpty(_authClientId))
                    return AuthId;
                else
                    return _authClientId;
            }
            set { _authClientId = value; }
        }

        public ClientBase()
        {
            SyncInvocationTimeout = TimeSpan.FromSeconds(30);
            _connection = new ConnectionBase();
            _connection.OnDataReceived += new Action<MsgType, byte[]>(_connection_OnDataReceived);
            _connection.OnError += new Action<string>(_connection_OnError);
        }

        public bool Connect(string address, int port)
        {
            lock(_syncObj)
            {
                bool connected = false;
                bool attachExisting = false;
                ConnectAsync(address, port, 
                    delegate(bool succ, string err, bool attach)
                    {
                        lock (_syncObj)
                        {
                            connected = succ;
                            attachExisting = attach;
                            Monitor.Pulse(_syncObj);
                        }
                    });

                bool wtSucc = Monitor.Wait(_syncObj, SyncInvocationTimeout);
                if (!wtSucc)
                    throw new InvocationTimeout("Connecting server time out");
                return connected;
            }
        }

        public void ConnectAsync(string addressWithPort, Action<bool, string, bool> connectCompletionHandler)
        {
            string[] tmp = addressWithPort.Split(':');
            int port = int.Parse(tmp[1]);
            ConnectAsync(tmp[0], port, connectCompletionHandler);
        }

        public void ConnectAsync(string address, int port, Action<bool, string, bool> connectCompletionHandler)
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
                            connectCompletionHandler(succ, err, false);
                    }
                });
        }

        public void Disconnect()
        {
            _connection.Close();
        }

        public byte[] Request(string method, byte[] reqData, int timeout = -1)
        {
            lock (_syncObj)
            {
                bool invokeSucc = false;
                byte[] ret_data = null;
                string invokeErr = string.Empty;
                RequestAsync(method, reqData,
                    delegate(bool succ, byte[] retData, string errMsg)
                    {
                        lock (_syncObj)
                        {
                            invokeSucc = succ;
                            ret_data = retData;
                            invokeErr = errMsg;
                            Monitor.Pulse(_syncObj);
                        }
                    });

                bool wtSucc;
                if (timeout > 0)
                    wtSucc = Monitor.Wait(_syncObj, timeout);
                else
                    wtSucc = Monitor.Wait(_syncObj);
                if (wtSucc)
                {
                    if (!invokeSucc)
                        throw new RequestResponseException(invokeErr);
                }
                else
                {
                    throw new InvocationTimeout(string.Format("Request '{0}' timeout", method));
                }
                return ret_data;
            }
        }

        public void RequestAsync(string method, byte[] reqData, ResponseHandlerDel responseHandler)
        {
            Packet.Request req = new Packet.Request()
            {
                session = SessionId,
                method = method,
                param_data = reqData
            };

            byte[] data = DataTranslater.Serialize(req);

            BookRequest(method, responseHandler);

            _connection.SendAsync(MsgType.REQ, data,
                delegate(bool succ, string msg)
                {
                    if (succ)
                    {
                        Trace.WriteLine(string.Format("Request {0} success.", method));
                    }
                    else
                    {
                        RemoveBookedRequest(method);
                        responseHandler(false, null, msg);
                    }
                });
        }

        protected abstract void DispatchCallback(string method, byte[] paramData);

        void _connection_OnDataReceived(MsgType msgType, byte[] receivedData)
        {
            switch (msgType)
            {
                case MsgType.CONN_ACK:
                    OnEstablishConnectionAck(receivedData);
                    break;
                case MsgType.RSP:
                    OnResponse(receivedData);
                    break;
                case MsgType.CALLBK_REQ:
                    OnCallbackReq(receivedData);
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

        private void EstablishConnection(Action<bool, string, bool> connectCompletionHandler)
        {
            Packet.Connect connReq = new Packet.Connect();
            connReq.userid = AuthClientId;
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
                            false, string.Format("Attempt to establish connection failed due to {0}", msg), false);
                    }
                });
        }

        private void OnEstablishConnectionAck(byte[] data)
        {
            bool attachExisting = false;
            if (data != null && data.Length > 0)
            {
                Packet.ConnectAck connAck = DataTranslater.Deserialize<Packet.ConnectAck>(data);
                IsConnected = connAck.success;
                SessionId = connAck.session;
                attachExisting = connAck.attach_existing;
            }

            Delegate callback = GetResponseCallback(typeof(Packet.Connect).Name);
            if (callback != null)
            {
                string errMsg = !IsConnected ? "交易终端拒绝连接" : "";
                Action<bool, string, bool> connCompletion = callback as Action<bool, string, bool>;
                if (connCompletion != null)
                    connCompletion(IsConnected, errMsg, attachExisting);
            }
        }

        private void OnResponse(byte[] data)
        {
            bool recvSucc = false;
            byte[] respData = null;
            string method = string.Empty;
            string errMsg = string.Empty;
            if (data != null && data.Length > 0)
            {
                Packet.Response resp = DataTranslater.Deserialize<Packet.Response>(data);
                method = resp.method;
                respData = resp.return_data;
                errMsg = resp.error;

                recvSucc = !string.IsNullOrEmpty(resp.method) && resp.invoke;
            }

            Delegate callback = GetResponseCallback(method);
            if (callback != null)
            {
                ResponseHandlerDel respCompletion = callback as ResponseHandlerDel;
                if (respCompletion != null)
                    respCompletion(recvSucc, respData, errMsg);
            }
        }

        private void OnCallbackReq(byte[] callbackReqData)
        {
            byte[] paramData = null;
            string method = string.Empty;
            if (callbackReqData != null && callbackReqData.Length > 0)
            {
                Packet.CallbackReq cbReq = DataTranslater.Deserialize<Packet.CallbackReq>(callbackReqData);
                method = cbReq.method;
                paramData = cbReq.param_data;

                DispatchCallback(method, paramData);                
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

    public class RequestResponseException : Exception
    {
        public RequestResponseException(string errMsg) : base(errMsg) 
        { }
    }

    public class InvocationTimeout : Exception
    {
        public InvocationTimeout(string errMsg) : base(errMsg) { }
    }
}

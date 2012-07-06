using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace APInvokeManaged
{
    class ConnectionBase
    {
        public enum Status { Closed, Open, Fault, Closing };

        public delegate void ConnectDoneCallback(bool succ, string err);
        public delegate void SendDoneCallback(bool succ, string err);
        public delegate void ReceiveDoneCallback(bool succ, string err, MsgType msgType, byte[] data);
        
        private const int HeaderBytesLength = 12;

        private Socket _tcpClient;
        // for receive data
        private MsgType _inbound_msg_type = MsgType.UNKNOWN;
        private byte[] _inbound_header = new byte[HeaderBytesLength];
        private byte[] _inbound_data = null;

        private string _address;
        private int _port;
        private Status _status = Status.Closed;
        private bool _isContinuousReading = false;
        
        public event Action<MsgType, byte[]> OnDataReceived;
        public event Action<string> OnError;

        public ConnectionBase(string address, int port)
        {
            _address = address;
            _port = port;
        }

        public ConnectionBase()
        {
        }

        public Status ConnectionStatus
        {
            get { return _status; }
        }

        public bool IsConnected
        {
            get { return _status == Status.Open; }
        }

        public string Address
        {
            get { return _address; }
            set 
            {
                if (IsConnected)
                    throw new InvalidOperationException("Cannot change address during connected");
                _address = value; 
            }
        }

        public int Port
        {
            get { return _port; }
            set 
            {
                if (IsConnected)
                    throw new InvalidOperationException("Cannot change port during connected");

                _port = value;
            }
        }

        public void OpenAsync(ConnectDoneCallback connectDoneCallback)
        {
            if (string.IsNullOrEmpty(Address))
                throw new InvalidOperationException("Address is not set");
            if (Port < 1 || Port > 65535)
                throw new InvalidOperationException("Port is not valid");
            if (IsConnected)
                throw new InvalidOperationException("Cannot connect again during connected");

            IPHostEntry hostEntry = Dns.GetHostEntry(Address);
            IPAddress ipAddr = hostEntry.AddressList.SingleOrDefault(h => h.AddressFamily == AddressFamily.InterNetwork);
            IPEndPoint ipe = new IPEndPoint(ipAddr, Port);

            _tcpClient =
                new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            Action<IAsyncResult> actionConnectDone = delegate(IAsyncResult ar)
            {
                bool succ = false;
                string err = string.Empty;
                if (ar.IsCompleted)
                {
                    try
                    {
                        _tcpClient.EndConnect(ar);
                        succ = true;
                        _status = Status.Open;
                    }
                    catch (System.Exception ex)
                    {
                       err = String.Format("Encouter error while trying to connect server due to {0}", ex.Message);
                    }
                }

                if (connectDoneCallback != null)
                    connectDoneCallback(succ, err);
            };

            _tcpClient.BeginConnect(ipe, new AsyncCallback(actionConnectDone), null);
        }

        public void SendAsync(MsgType msgType, byte[] data, SendDoneCallback callback)
        {
            if (msgType == MsgType.UNKNOWN)
            {
                callback.BeginInvoke(false, "message type is unknown", null, null);
                return;
            }

            if (data.Length == 0)
            {
                callback.BeginInvoke(false, "data length is 0", null, null);
                return;
            }

            string sHeader = string.Format("{0,4:X}{1,8:X}", (int)msgType, data.Length);
            byte[] headerbytes = System.Text.ASCIIEncoding.ASCII.GetBytes(sHeader);
            System.Diagnostics.Debug.Assert(headerbytes.Length == HeaderBytesLength);

            List<ArraySegment<byte>> buffers = new List<ArraySegment<byte>>();
            buffers.Add(new ArraySegment<byte>(headerbytes));
            buffers.Add(new ArraySegment<byte>(data));

            Action<IAsyncResult> sendDoneAction = delegate(IAsyncResult ar)
            {
                bool succ = false;
                string err = string.Empty;
                if (ar.IsCompleted)
                {
                    try
                    {
                        int sendBytes = _tcpClient.EndSend(ar);
                        succ = true;
                    }
                    catch (System.Exception ex)
                    {
                        err = string.Format("Encouter error while sending data due to {0}", ex.Message);
                    }
                }

                if (callback != null)
                    callback(succ, err);
            };

            _tcpClient.BeginSend(buffers, SocketFlags.None, new AsyncCallback(sendDoneAction), null);
        }

        public void ReceiveAsync(ReceiveDoneCallback callback)
        {
            _tcpClient.BeginReceive(_inbound_header, 0, HeaderBytesLength,
                 SocketFlags.None, new AsyncCallback(OnReceiveHeader), callback);
        }

        private void OnReceiveHeader(IAsyncResult ar)
        {
            bool succ = false;
            string err = string.Empty;

            if (ar.IsCompleted)
            {
                try
                {
                    int recvBytes = _tcpClient.EndReceive(ar);
                    
                    if (!IsConnected) return;   // remote call close

                    System.Diagnostics.Debug.Assert(recvBytes == HeaderBytesLength);

                    // parse header buffer

                    // get message type
                    string t = System.Text.ASCIIEncoding.ASCII.GetString(_inbound_header, 0, 4);
                    _inbound_msg_type = (MsgType)Convert.ToInt32(t.Trim(), 16);
                    // get payload size
                    string size = System.Text.ASCIIEncoding.ASCII.GetString(_inbound_header, 4, 8);
                    int data_size = Convert.ToInt32(size.Trim(), 16);
                    // create buffer for receive payload data
                    _inbound_data = new byte[data_size];

                    succ = true;
                }
                catch(SocketException socketEx)
                {
                    err = string.Format("Socket Error: {0}", socketEx.Message);
                    _status = Status.Fault;
                }
                catch (System.Exception ex)
                {
                    err = string.Format("Encouter error while receiving header due to {0}", ex.Message);
                }
            }

            if (succ)
            {
                _tcpClient.BeginReceive(_inbound_data, 0, _inbound_data.Length,
                    SocketFlags.None, new AsyncCallback(OnReceiveDataBody), ar.AsyncState);
            }
            else
            {
                ReceiveDoneCallback callback = ar.AsyncState as ReceiveDoneCallback;
                if (callback != null)
                    callback(succ, err, MsgType.UNKNOWN, new byte[0]);
            }
        }

        private void OnReceiveDataBody(IAsyncResult ar)
        {
            bool succ = false;
            string err = string.Empty;
            ReceiveDoneCallback callback = ar.AsyncState as ReceiveDoneCallback;
            if (ar.IsCompleted)
            {
                try
                {
                    int recvBytes = _tcpClient.EndReceive(ar);

                    if (!IsConnected) return;   // remote call close

                    System.Diagnostics.Debug.Assert(recvBytes == _inbound_data.Length);
                    succ = true;

                    if (callback != null)
                    {
                        callback(succ, err, _inbound_msg_type, _inbound_data);
                    }
                }
                catch (SocketException socketEx)
                {
                    err = string.Format("Socket Error: {0}", socketEx.Message);
                    _status = Status.Fault;
                }
                catch (System.Exception ex)
                {
                    err = string.Format("Encouter error while receiving data payload due to {0}", ex.Message);
                }
            }

            if (!succ)
            {
                if (callback != null)
                    callback(succ, err, MsgType.UNKNOWN, new byte[0]);
            }

            if (_isContinuousReading)
                ReceiveAsync(callback);
        }

        public void StartReading()
        {
            Action<bool, string, MsgType, byte[]> dataReadyAction = delegate(bool succ, string err, MsgType msgType, byte[] data)
            {
                if (succ)
                {
                    RaiseDataReceived(msgType, data);
                }
                else
                {
                    RaiseError(err);
                    if (!IsConnected)   // don't read socket any more once error
                        _isContinuousReading = false;
                }
            };
            _isContinuousReading = true;
            ReceiveAsync(new ReceiveDoneCallback(dataReadyAction));
        }

        public void StopReading()
        {
            _isContinuousReading = false;
        }

        private void RaiseDataReceived(MsgType msg_type, byte[] data)
        {
            if (OnDataReceived != null)
                OnDataReceived(msg_type, data);
        }

        private void RaiseError(string errMsg)
        {
            if (OnError != null)
                OnError(errMsg);
        }

        public void Close()
        {
            Action<IAsyncResult> disconnectDoneAction = delegate(IAsyncResult ar)
            {
                _status = Status.Closed;
            };
            _status = Status.Closing;
            _tcpClient.BeginDisconnect(false, new AsyncCallback(disconnectDoneAction), null);
        }
    }
}

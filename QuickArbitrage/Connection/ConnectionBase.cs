using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using QuickArbitrage.Common;
using QuickArbitrage.Connection.DataEntity;

namespace QuickArbitrage.Connection
{
    public delegate void ConnectDoneCallback(bool succ, string err);
    public delegate void SendDoneCallback(bool succ, string err);
    public delegate void ReceiveDoneCallback(bool succ, string err, MsgType msgType, byte[] data);

    public class ConnectionBase
    {
        private const string ServerAddress = "127.0.0.1";
        private const int Port = 18168;
        private const int HeaderBytesLength = 12;

        private Socket _tcpClient;

        private bool _isConnected = false;
        private bool _isContinuousReading = false;

        // for receive data
        private MsgType _inbound_msg_type = MsgType.UNKNOWN;
        private byte[] _inbound_header = new byte[HeaderBytesLength];
        private byte[] _inbound_data = null;

        public void ConnectAsync(ConnectDoneCallback connectDoneCallback)
        {
            IPHostEntry hostEntry = Dns.GetHostEntry(ServerAddress);
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
                        _isConnected = succ = true;
                    }
                    catch (System.Exception ex)
                    {
                        LogManager.Logger.ErrorFormat("Encouter error while trying to connect server due to {0}", ex);
                        err = ex.Message;
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
                        LogManager.Logger.ErrorFormat("Encouter error while sending data due to {0}", ex);
                        err = ex.Message;
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
                SocketFlags.None, new AsyncCallback(OnReceiveHader), callback);
        }

        private void OnReceiveHader(IAsyncResult ar)
        {
            bool succ = false;
            string err = string.Empty;

            if (ar.IsCompleted)
            {
                try
                {
                    int recvBytes = _tcpClient.EndReceive(ar);
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
                catch (System.Exception ex)
                {
                    LogManager.Logger.ErrorFormat("Encouter error while receiving header due to {0}", ex);
                    err = ex.Message;
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
                    System.Diagnostics.Debug.Assert(recvBytes == _inbound_data.Length);
                    succ = true;

                    if (callback != null)
                    {
                        callback(succ, err, _inbound_msg_type, _inbound_data);
                    }
                }
                catch (System.Exception ex)
                {
                    LogManager.Logger.ErrorFormat("Encouter error while receiving data payload due to {0}", ex);
                    err = ex.Message;
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
                    OnDataReceived(msgType, data);
                }
                else
                {
                    LogManager.Logger.ErrorFormat("Error occurs while receiving data:{0}", err);
                }
            };
            _isContinuousReading = true;
            ReceiveAsync(new ReceiveDoneCallback(dataReadyAction));
        }

        public void StopReading()
        {
            _isContinuousReading = false;
        }

        protected virtual void OnDataReceived(MsgType msg_type, byte[] data)
        {
            
        }

        public void Disconnect()
        {
            Action<IAsyncResult> disconnectDoneAction = delegate(IAsyncResult ar)
            {
                _isConnected = false;
            };

            _tcpClient.BeginDisconnect(false, new AsyncCallback(disconnectDoneAction), null);
        }
    }
}

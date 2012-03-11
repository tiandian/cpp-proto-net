using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace QuickArbitrage.Connection
{
    public class ConnectionBase
    {
        private const string ServerAddress = "127.0.0.1";
        private const int Port = 18168;

        private Socket _tcpClient;

        private bool _isConnected = false;

        public bool Connect()
        {
            IPHostEntry hostEntry = Dns.GetHostEntry(ServerAddress);

            IPAddress ipAddr = hostEntry.AddressList.SingleOrDefault(h => h.AddressFamily == AddressFamily.InterNetwork);

            IPEndPoint ipe = new IPEndPoint(ipAddr, Port);
            _tcpClient =
                new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            _tcpClient.Connect(ipe);

            _isConnected = _tcpClient.Connected;
            

            string s = string.Format("{0,4:X}{1,8:X}", 4, 256);

            byte[] bb = System.Text.ASCIIEncoding.ASCII.GetBytes(s);

//            _tcpClient.Connect("127.0.0.1", 18168);

            byte[] buf = new byte[12];
            _tcpClient.Receive(buf);

            string t = System.Text.ASCIIEncoding.ASCII.GetString(buf, 0, 4);
            int msg_type = int.Parse(t);
            string size = System.Text.ASCIIEncoding.ASCII.GetString(buf, 4, 8);
            int data_size = int.Parse(size);

            byte[] data_buf = new byte[data_size];
            _tcpClient.Receive(data_buf);
            string s_data = System.Text.ASCIIEncoding.ASCII.GetString(data_buf);

            Socket readSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            readSocket.Connect(ipe);

            return true;
        }

        public void Disconnect()
        {
            _tcpClient.Close();
        }

        
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using ProtoBuf;
using QuickArbitrage.Connection.DataEntity;

namespace QuickArbitrage.Connection
{
    partial class Client
    {
        protected byte[] Serialize<T>(T obj) where T : class
        {
            byte[] data = null;
            using (MemoryStream mstream = new MemoryStream())
            {
                Serializer.Serialize(mstream, obj);
                data = mstream.ToArray();
            }
            return data;
        }

        protected T Deserialize<T>(byte[] data) 
            where T : class, new()
        {
            T t = default(T);
            using (MemoryStream ms = new MemoryStream(data))
            {
                t = Serializer.Deserialize<T>(ms);
            }
            return t;
        }

        protected override void OnDataReceived(MsgType msg_type, byte[] data)
        {
            switch (msg_type)
            {
                case MsgType.SYS_HEARTBEAT_ACK:
                    break;
                case MsgType.RSP_LOGIN:
                    OnLoginResponse(Deserialize<RspLogin>(data));
                    break;
                case MsgType.RSP_SUBSCRIBE:
                    break;
                case MsgType.RSP_UNSUBSCRIBE:
                    break;
                case MsgType.QUOTE:
                    break;
                default:
                    break;
            }
        }

    }
}

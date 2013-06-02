using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using ProtoBuf;

namespace APInvokeManaged
{
    public static class DataTranslater
    {
        public static byte[] Serialize<T>(T obj) where T : class
        {
            byte[] data = null;
            using (MemoryStream mstream = new MemoryStream())
            {
                Serializer.Serialize(mstream, obj);
                data = mstream.ToArray();
            }
            return data;
        }

        public static T Deserialize<T>(byte[] data)
            where T : class, new()
        {
            T t = default(T);
            using (MemoryStream ms = new MemoryStream(data))
            {
                t = Serializer.Deserialize<T>(ms);
            }
            return t;
        }
    }
}

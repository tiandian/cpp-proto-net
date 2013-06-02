using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using ProtoBuf;
using QuickArbitrage.Common;

namespace QuickArbitrage.Connection.TransferTest
{
    public static class StreamFileTest
    {
        public static void WriteCustomer(string path)
        {
            Customer cust = Customer.Invent();

            using (Stream file = File.Create(path))
            {
                Serializer.Serialize(file, cust);
                file.Close();
            }

            LogManager.Logger.InfoFormat("{0}: {1} bytes",
                Path.GetFileName(path), new FileInfo(path).Length); 
        }

        public static Customer ReadCustomer(string path)
        {
            Customer newPerson; 
            using (var file = File.OpenRead(path)) 
            {
                newPerson = Serializer.Deserialize<Customer>(file); 
            }
            return newPerson;
        }
    }
}

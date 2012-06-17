using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using APInvokeManaged;
using ManagedClient.Entity;

namespace ManagedClient
{
    class Client : ClientBase
    {
        public bool TestHello(string p1, string p2)
        {
            TestHelloParams tp = new TestHelloParams()
            {
                 param1 = "aaa",
                 param2 = "bbb"
            };
            byte[] param_data = DataTranslater.Serialize(tp);

            byte[] ret_data = Request("TestHello", param_data);
            if (ret_data != null)
            {
                TestHelloReturn rt = DataTranslater.Deserialize<TestHelloReturn>(ret_data);
                return rt.success;
            }
            else
                throw new Exception("Returned data is invalid"); 
        }

        protected override void DispatchMessage()
        {
            
        }
    }
}

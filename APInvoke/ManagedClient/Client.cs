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
        public event Action<string, string> OnCallbackEvent;

        public bool TestHello(string p1, string p2)
        {
            TestHelloParams tp = new TestHelloParams()
            {
                 param1 = p1,
                 param2 = p2
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

        protected override void DispatchCallback(string method, byte[] paramData)
        {
            if (method == "TestCallback")
            {
                if (OnCallbackEvent != null)
                {
                    TestCallbackParams cbParams = DataTranslater.Deserialize<TestCallbackParams>(paramData);
                    OnCallbackEvent(cbParams.param3, cbParams.param4);
                }
            }
        }
    }
}

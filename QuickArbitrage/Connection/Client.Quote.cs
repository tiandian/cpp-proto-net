using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common.Contract;
using System.Threading;
using QuickArbitrage.Connection.DataEntity;
using QuickArbitrage.Common;

namespace QuickArbitrage.Connection
{
    partial class Client : IQuoteClient
    {
        public void Subscribe(IEnumerable<string> symbols)
        {
            ReqSubscribe reqSub = new ReqSubscribe();
            reqSub.Symbols.AddRange(symbols);
            byte[] data = Serialize(reqSub);

            SendAsync(MsgType.REQ_SUBSCRIBE, data, new SendDoneCallback(
                (succ, msg) => {
                    if (succ)
                    {
                        LogManager.Logger.Debug("Request subscribe sent.");
                    }
                    else
                    {
                        LogManager.Logger.WarnFormat("Request subscribe sending failed due to {0}", msg);
                    }
                }));
        }

        public void Unsubscribe(IEnumerable<string> symbols)
        {
            ReqUnsubscribe reqUnSub = new ReqUnsubscribe();
            reqUnSub.Symbols.AddRange(symbols);
            byte[] data = Serialize(reqUnSub);

            SendAsync(MsgType.REQ_UNSUBSCRIBE, data, new SendDoneCallback(
                (succ, msg) =>
                {
                    if (succ)
                    {
                        LogManager.Logger.Debug("Request UnSubscribe sent.");
                    }
                    else
                    {
                        LogManager.Logger.WarnFormat("Request UnSubscribe sending failed due to {0}", msg);
                    }
                }));
        }

        public event EventHandler<OnQuoteReceivedEventArgs> OnQuoteReceived;

        public void HandleReceviedQuote(Quote q)
        {
            ThreadPool.QueueUserWorkItem(delegate {

                // Simply using thread pool instead of dedicated buffer runner
                if (OnQuoteReceived != null)
                {
                    OnQuoteReceived(this, new OnQuoteReceivedEventArgs(q));
                }

            });
        }

        public void OnSubscribeResponse(RspSubscribe rspSub)
        {
            if (rspSub.Success)
                LogManager.Logger.Info("Subscribe quote succeeded.");
            else
                LogManager.Logger.Error("Subscribe quote failed");
        }

        public void OnUnSubscribeResponse(RspUnsubscribe rspUnSub)
        {
            if (rspUnSub.Success)
                LogManager.Logger.Info("UnSubscribe quote succeeded.");
            else
                LogManager.Logger.Error("UnSubscribe quote failed");
        }
    }
}

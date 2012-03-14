using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Common.Contract
{
    public interface IQuoteClient
    {
        void Subscribe(IEnumerable<string> symbols);

        void Unsubscribe(IEnumerable<string> symbols);

        event EventHandler<OnQuoteReceivedEventArgs> OnQuoteReceived;
    }

    public class OnQuoteReceivedEventArgs : EventArgs
    {
        public Quote Quote { get; private set; }

        public string Symbol
        {
            get 
            {
                if (Quote != null)
                    return Quote.symbol;
                else
                    return string.Empty;
            }
        }

        public OnQuoteReceivedEventArgs(Quote q)
        {
            this.Quote = q;
        }
    }
}

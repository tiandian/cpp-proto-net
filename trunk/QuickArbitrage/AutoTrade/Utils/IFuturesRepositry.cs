using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Common;

namespace AutoTrade.MainFrame.Utils
{
    public interface IFuturesRepositry
    {
        IEnumerable<Future> AllFutures { get; }

        IEnumerable<ExchangeInfo> AllExchanges { get; }

        IEnumerable<Future> GetFuturesByExchange(ExchangeID exId);
    }
}

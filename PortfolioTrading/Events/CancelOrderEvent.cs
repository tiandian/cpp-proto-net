using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class CancelOrderEvent : CompositePresentationEvent<CancelOrderEventArgs>
    {
    }

    public class CancelOrderEventArgs
    {
        public string AccountId { get; set; }
        public trade.Order OrderToCancel { get; set; }
    }
}

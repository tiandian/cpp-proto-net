using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class TradeUpdatedEvent : CompositePresentationEvent<trade.Trade>
    {
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class MultiLegOrderUpdatedEvent : CompositePresentationEvent<MultiLegOrderUpdateArgs>
    {
    }

    public class MultiLegOrderUpdateArgs
    {
        public string AccountId { get; set; }
        public trade.MultiLegOrder MultiLegOrder { get; set; }
    }
}

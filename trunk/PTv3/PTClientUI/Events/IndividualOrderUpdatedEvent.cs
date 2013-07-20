using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class IndividualOrderUpdatedEvent : CompositePresentationEvent<OrderUpdateArgs>
    {
    }

    public class OrderUpdateArgs
    {
        public string AccountId { get; set; }

        public string PortfolioId { get; set; }

        public string MlOrderId { get; set; }

        public string LegOrderRef { get; set; }

        public PTEntity.Order LegOrder { get; set; }
    }
}

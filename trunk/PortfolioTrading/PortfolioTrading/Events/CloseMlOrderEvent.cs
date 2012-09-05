using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class CloseMlOrderEvent : CompositePresentationEvent<CloseMlOrderArgs>
    {
    }

    public class CloseMlOrderArgs
    {
        public string MlOrderId { get; set; }
        public string LegOrderRef { get; set; }
    }
}

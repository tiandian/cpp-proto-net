using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PortfolioTrading.Events
{
    public class ManualOpenDirectionChangeEvent : CompositePresentationEvent<PortfolioVM>
    {
    }
}

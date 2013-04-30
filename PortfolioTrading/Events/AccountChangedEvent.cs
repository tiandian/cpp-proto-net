using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Events
{
    public class AccountChangedEvent : CompositePresentationEvent<AccountVM>
    {
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PortfolioTrading.Modules.Account;
using Microsoft.Practices.Prism.Events;

namespace PortfolioTrading.Events
{
    public class AccountSelectedEvent : CompositePresentationEvent<AccountVM>
    {
    }
}

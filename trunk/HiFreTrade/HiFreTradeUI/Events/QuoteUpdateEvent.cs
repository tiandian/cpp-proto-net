using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Events;

namespace HiFreTradeUI.Events
{
    public class QuoteUpdateEvent : CompositePresentationEvent<ViewModels.QuoteItem>
    {
    }
}

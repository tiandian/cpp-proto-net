using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using System.Windows.Threading;

namespace HiFreTradeUI
{
    [Export]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class UIThread
    {
        public Dispatcher Dispatcher { get; set; }
    }
}

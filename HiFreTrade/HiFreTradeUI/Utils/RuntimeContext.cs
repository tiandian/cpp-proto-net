using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;

namespace HiFreTradeUI.Utils
{
    [Export]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class RuntimeContext
    {
    }
}

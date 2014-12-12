using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PortfolioTrading.Utils
{
    public class RefDataItem<Tv, Td>
    {
        public Tv Value { get; set; }
        public Td DisplayText { get; set; }
    }

    public class RefDatItemsSource<Tv, Td> : List< RefDataItem<Tv, Td> >
    { }
}

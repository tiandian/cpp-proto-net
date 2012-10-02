using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Infragistics.Windows.DockManager;

namespace PortfolioTrading.Modules.Portfolio
{
    public class PortfolioContentPane : ContentPane
    {
        private string _viewName;

        public string ViewName
        {
            get { return _viewName; }
            set 
            { 
                _viewName = value;
                this.Header = string.Format("组合一览 [{0}]", _viewName);
            }
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Infragistics.Windows.DockManager;

namespace PortfolioTrading.Modules.Portfolio
{
    public abstract class MyContentPane : ContentPane
    {
        public abstract string ViewName { get; set; }
    }

    public class PortfolioContentPane : MyContentPane
    {
        private string _viewName;

        public override string ViewName
        {
            get { return _viewName; }
            set 
            { 
                _viewName = value;
                this.Header = string.Format("组合一览 [{0}]", _viewName);
            }
        }

    }

    public class PositionContentPane : MyContentPane
    {
        private string _viewName;

        public override string ViewName
        {
            get { return _viewName; }
            set
            {
                _viewName = value;
                this.Header = string.Format("帐户持仓 [{0}]", _viewName);
            }
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;

namespace QuickArbitrage.Main.ViewModel
{
    public class Strategy : BaseViewModel
    {
        #region StrategyID
        
        private string _stragegyID;
        public string StrategyID
        {
        	get { return _stragegyID; }
        	set
        	{
        		if (_stragegyID != value)
        		{
        			_stragegyID = value;
        			this.OnPropertyChanged("StrategyID");
        		}
        	}
        }
        #endregion

        #region Name
        
        private string _name;
        public string Name
        {
        	get { return _name; }
        	set
        	{
        		if (_name != value)
        		{
        			_name = value;
        			this.OnPropertyChanged("Name");
        		}
        	}
        }
        #endregion
    }
}

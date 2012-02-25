using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.ViewModel
{
    class PositionItem : BaseViewModel
    {
        #region Code
        
        private string _code;
        public string Code
        {
        	get { return _code; }
        	set
        	{
        		if (_code != value)
        		{
        			_code = value;
        			this.OnPropertyChanged("Code");
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

        #region AvgCost
        
        private float _avgCost;
        public float AvgCost
        {
        	get { return _avgCost; }
        	set
        	{
        		if (!_avgCost.equals(value))
        		{
        			_avgCost = value;
        			this.OnPropertyChanged("AvgCost");
        		}
        	}
        }
        #endregion

        #region Quantity
        
        private int _quantity;
        public int Quantity
        {
        	get { return _quantity; }
        	set
        	{
        		if (_quantity != value)
        		{
        			_quantity = value;
        			this.OnPropertyChanged("Quantity");
        		}
        	}
        }
        #endregion

        #region Side
        
        private Side _side;
        public Side Side
        {
        	get { return _side; }
        	set
        	{
        		if (_side != value)
        		{
        			_side = value;
        			this.OnPropertyChanged("Side");
        		}
        	}
        }
        #endregion

        #region Gain
        
        private float _gain;
        public float Gain
        {
        	get { return _gain; }
        	set
        	{
        		if (!_gain.equals(value))
        		{
        			_gain = value;
        			this.OnPropertyChanged("Gain");
        		}
        	}
        }
        #endregion
    }
}

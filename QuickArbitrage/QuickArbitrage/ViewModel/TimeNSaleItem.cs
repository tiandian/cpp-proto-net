using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.ViewModel
{
    class TimeNSaleItem : BaseViewModel
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

        #region Price
        
        private float _price;
        public float Price
        {
        	get { return _price; }
        	set
        	{
        		if (!_price.equals(value))
        		{
        			_price = value;
        			this.OnPropertyChanged("Price");
        		}
        	}
        }
        #endregion

        #region Action
        
        private TradeAction _action;
        public TradeAction Action
        {
        	get { return _action; }
        	set
        	{
        		if (_action != value)
        		{
        			_action = value;
        			this.OnPropertyChanged("Action");
        		}
        	}
        }
        #endregion

        #region ExecuteTime
        
        private DateTime _executeTime;
        public DateTime ExecuteTime
        {
        	get { return _executeTime; }
        	set
        	{
        		if (_executeTime != value)
        		{
        			_executeTime = value;
        			this.OnPropertyChanged("ExecuteTime");
        		}
        	}
        }
        #endregion
    }
}

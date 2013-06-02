using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.ViewModel
{
    public class EquityItem : BaseViewModel
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

        #region Last

        private float _last;
        public float Last
        {
            get { return _last; }
            set
            {
                if (!_last.equals(value))
                {
                    _last = value;
                    this.OnPropertyChanged("Last");

                    Change = _last - PrevClose;
                    if (Side != Side.Unopen && Cost > 0)
                    {
                        Gain = Side == Side.Long ? _last - Cost : Cost - _last;
                    }
                }
            }
        }
        #endregion

        #region PrevClose
        
        private float _prevClose;
        public float PrevClose
        {
        	get { return _prevClose; }
        	set
        	{
        		if (!_prevClose.equals(value))
        		{
        			_prevClose = value;
        			this.OnPropertyChanged("PrevClose");
        		}
        	}
        }
        #endregion

        #region Change

        private float _change;
        public float Change
        {
            get { return _change; }
            private set
            {
                if (!_change.equals(value))
                {
                    _change = value;
                    this.OnPropertyChanged("Change");

                    PercentChg = _change / PrevClose;
                }
            }
        }
        #endregion

        #region PercentChg

        private float _percentChg;
        public float PercentChg
        {
            get { return _percentChg; }
            private 
            set
            {
                if (!_percentChg.equals(value))
                {
                    _percentChg = value;
                    this.OnPropertyChanged("PercentChg");
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

        #region Cost
        
        private float _cost;
        public float Cost
        {
        	get { return _cost; }
        	set
        	{
        		if (!_cost.equals(value))
        		{
        			_cost = value;
        			this.OnPropertyChanged("Cost");
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
        		if (_gain != value)
        		{
        			_gain = value;
        			this.OnPropertyChanged("Gain");
        		}
        	}
        }
        #endregion

        #region Ask
        
        private double _ask;
        public double Ask
        {
        	get { return _ask; }
        	set
        	{
        		if (_ask != value)
        		{
        			_ask = value;
        			this.OnPropertyChanged("Ask");
        		}
        	}
        }
        #endregion

        #region Bid
        
        private double _bid;
        public double Bid
        {
        	get { return _bid; }
        	set
        	{
        		if (_bid != value)
        		{
        			_bid = value;
        			this.OnPropertyChanged("Bid");
        		}
        	}
        }
        #endregion

        #region LegStatus
        
        private string _legStatus;
        public string LegStatus
        {
        	get { return _legStatus; }
        	set
        	{
        		if (_legStatus != value)
        		{
        			_legStatus = value;
        			this.OnPropertyChanged("LegStatus");
        		}
        	}
        }
        #endregion
    }
}

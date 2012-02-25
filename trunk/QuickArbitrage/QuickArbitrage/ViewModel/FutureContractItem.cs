using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Main.ViewModel
{
    class FutureContractItem : QuickArbitrage.Main.Util.BaseViewModel
    {
        #region FurtureCode 

        private string _futureCode = string.Empty;
        public string FutureCode
        {
            get { return _futureCode; }
            set
            {
                if (_futureCode != value)
                {
                    _futureCode = value;
                    this.OnPropertyChanged("FutureCode");
                }
            }
        } 
        #endregion

        #region FutureName

        private string _futureName;
        public string FutureName
        {
            get { return _futureName; }
            set
            {
                if (_futureName != value)
                {
                    _futureName = value;
                    this.OnPropertyChanged("FutureName");
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
                if (_last != value)
                {
                    _last = value;
                    this.OnPropertyChanged("Last");
                }
            }
        } 
        #endregion

        #region Change

        private float _change;
        public float Change
        {
            get { return _change; }
            set
            {
                if (_change != value)
                {
                    _change = value;
                    this.OnPropertyChanged("Change");
                }
            }
        } 
        #endregion

        #region PercentChg

        private float _percentChg;
        public float PercentChg
        {
            get { return _percentChg; }
            set
            {
                if (_percentChg != value)
                {
                    _percentChg = value;
                    this.OnPropertyChanged("PercentChg");
                }
            }
        } 
        #endregion

        #region Ask

        private float _ask;
        public float Ask
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
        
        private float _bid;
        public float Bid
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

        #region IntradayInc 日增仓
        
        private int _intradayInc;
        public int IntradayInc
        {
        	get { return _intradayInc; }
        	set
        	{
        		if (_intradayInc != value)
        		{
        			_intradayInc = value;
        			this.OnPropertyChanged("IntradayInc");
        		}
        	}
        }
        #endregion

        #region IntradayVol 总手
        
        private uint _intradayVol;
        public uint IntradayVol
        {
        	get { return _intradayVol; }
        	set
        	{
        		if (_intradayVol != value)
        		{
        			_intradayVol = value;
        			this.OnPropertyChanged("IntradayVol");
        		}
        	}
        }
        #endregion

        #region MarketPosition 持仓量
        
        private uint _marketPostion;
        public uint MarketPosition
        {
        	get { return _marketPostion; }
        	set
        	{
        		if (_marketPostion != value)
        		{
        			_marketPostion = value;
        			this.OnPropertyChanged("MarketPosition");
        		}
        	}
        }
        #endregion

        #region Open
        
        private float _open;
        public float Open
        {
        	get { return _open; }
        	set
        	{
        		if (_open != value)
        		{
        			_open = value;
        			this.OnPropertyChanged("Open");
        		}
        	}
        }
        #endregion

        #region High
        
        private float _high;
        public float High
        {
        	get { return _high; }
        	set
        	{
        		if (_high != value)
        		{
        			_high = value;
        			this.OnPropertyChanged("High");
        		}
        	}
        }
        #endregion

        #region Low
        
        private float _low;
        public float Low
        {
        	get { return _low; }
        	set
        	{
        		if (_low != value)
        		{
        			_low = value;
        			this.OnPropertyChanged("Low");
        		}
        	}
        }
        #endregion

        #region Close 昨收
        
        private float _close;
        public float Close
        {
        	get { return _close; }
        	set
        	{
        		if (_close != value)
        		{
        			_close = value;
        			this.OnPropertyChanged("Close");
        		}
        	}
        }
        #endregion

        #region ClearPrice 结算价
        
        private float _clearPrice;
        public float ClearPrice
        {
        	get { return _clearPrice; }
        	set
        	{
        		if (_clearPrice != value)
        		{
        			_clearPrice = value;
        			this.OnPropertyChanged("ClearPrice");
        		}
        	}
        }
        #endregion

        #region PrevClearPrice 昨结算价
        
        private float _prevClearPrice;
        public float PrevClearPrice
        {
        	get { return _prevClearPrice; }
        	set
        	{
        		if (_prevClearPrice != value)
        		{
        			_prevClearPrice = value;
        			this.OnPropertyChanged("PrevClearPrice");
        		}
        	}
        }
        #endregion

        #region AboveAskDeal 外盘
        
        private uint _aboveAskDeal;
        public uint AboveAskDeal
        {
        	get { return _aboveAskDeal; }
        	set
        	{
        		if (_aboveAskDeal != value)
        		{
        			_aboveAskDeal = value;
        			this.OnPropertyChanged("AboveAskDeal");
        		}
        	}
        }
        #endregion

        #region BelowBidDeal 内盘
        
        private uint _belowBidDeal;
        public uint BelowBidDeal
        {
        	get { return _belowBidDeal; }
        	set
        	{
        		if (_belowBidDeal != value)
        		{
        			_belowBidDeal = value;
        			this.OnPropertyChanged("BelowBidDeal");
        		}
        	}
        }
        #endregion
    }
}

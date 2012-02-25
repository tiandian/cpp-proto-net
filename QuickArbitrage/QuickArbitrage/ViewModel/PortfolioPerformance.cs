using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;

namespace QuickArbitrage.Main.ViewModel
{
    class PortfolioPerformance : BaseViewModel
    {
        #region TradeCount
        
        private int _tradeCount;
        public int TradeCount
        {
        	get { return _tradeCount; }
        	set
        	{
        		if (_tradeCount != value)
        		{
        			_tradeCount = value;
        			this.OnPropertyChanged("TradeCount");
        		}
        	}
        }
        #endregion

        #region SuccessCount
        
        private int _successCount;
        public int SuccessCount
        {
        	get { return _successCount; }
        	set
        	{
        		if (_successCount != value)
        		{
        			_successCount = value;
        			this.OnPropertyChanged("SuccessCount");
        		}
        	}
        }
        #endregion

        #region TotalGain
        
        private float _totalGain;
        public float TotalGain
        {
        	get { return _totalGain; }
        	set
        	{
        		if (_totalGain != value)
        		{
        			_totalGain = value;
        			this.OnPropertyChanged("TotalGain");
        		}
        	}
        }
        #endregion
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class PositionDetailVM
    {
        private ObservableCollection<PositionDetailItem> _positionDetailItems = new ObservableCollection<PositionDetailItem>();

        public IEnumerable<PositionDetailItem> PositionDetailItems
        {
            get { return _positionDetailItems; }
        }

        public void Clear()
        {
            _positionDetailItems.Clear();
        }

        public void AddDetailItem(Win32Invoke.PositionDetail posiDetail)
        {
            PositionDetailItem posiItem = new PositionDetailItem();

            posiItem.Symbol = posiDetail.caSymbol;
            posiItem.HedgeFlag = GetHedgeText(posiDetail.cHedgeFlag);
            posiItem.Direction = GetDirection(posiDetail.iDirection);
            DateTime day;
            bool succ = DateTime.TryParse(posiDetail.caOpenDate, out day);
            if(succ) posiItem.OpenDate = day;
            posiItem.Volume = posiDetail.iVolume;
            posiItem.OpenPrice = posiDetail.dOpenPrice;
            succ = DateTime.TryParse(posiDetail.caTradingDay, out day);
            if (succ) posiItem.TradingDay = day;
            posiItem.ExchangeID = posiDetail.caExchangeID;
            posiItem.CloseProfit = posiDetail.dCloseProfit;
            posiItem.PositionProfit = posiDetail.dPositionProfit;
            posiItem.Margin = posiDetail.dMargin;
            posiItem.MarginRateByMoney = posiDetail.dMarginRateByMoney;
            posiItem.CloseVolume = posiDetail.CloseVolume;
            posiItem.CloseAmount = posiDetail.CloseAmount;

            _positionDetailItems.Add(posiItem);
        }

        private static string GetHedgeText(char hedgeFlag)
        {
            ///投机
            ///#define THOST_FTDC_HF_Speculation '1'
            ///套利
            ///#define THOST_FTDC_HF_Arbitrage '2'
            ///套保
            ///#define THOST_FTDC_HF_Hedge '3'
            switch(hedgeFlag)
            {
                case '1':
                    return "投机";
                case '2':
                    return "套利";
                case '3':
                    return "套保";
                default:
                    return "";
            }
        }

        private static string GetDirection(int iDirection)
        {
            switch (iDirection)
            {
                case 1:
                    return "空头";
                case 2:
                    return "多头";
                default:
                    return "未知";
            }
        }
    }

    public class PositionDetailItem : NotificationObject
    {
        #region Symbol
        private string _symbol;

        public string Symbol
        {
            get { return _symbol; }
            set
            {
                if (_symbol != value)
                {
                    _symbol = value;
                    RaisePropertyChanged("Symbol");
                }
            }
        }
        #endregion

        #region HedgeFlag
        private string _hedgeFlag;

        public string HedgeFlag
        {
            get { return _hedgeFlag; }
            set
            {
                if (_hedgeFlag != value)
                {
                    _hedgeFlag = value;
                    RaisePropertyChanged("HedgeFlag");
                }
            }
        }
        #endregion

        #region Direction
        private string _direction;

        public string Direction
        {
            get { return _direction; }
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    RaisePropertyChanged("Direction");
                }
            }
        }
        #endregion

        #region OpenDate
        private DateTime _openDate;

        public DateTime OpenDate
        {
            get { return _openDate; }
            set
            {
                if (_openDate != value)
                {
                    _openDate = value;
                    RaisePropertyChanged("OpenDate");
                }
            }
        }
        #endregion

        #region Volume
        private int _volume;

        public int Volume
        {
            get { return _volume; }
            set
            {
                if (_volume != value)
                {
                    _volume = value;
                    RaisePropertyChanged("Volume");
                }
            }
        }
        #endregion

        #region OpenPrice
        private double _openPrice;

        public double OpenPrice
        {
            get { return _openPrice; }
            set
            {
                if (_openPrice != value)
                {
                    _openPrice = value;
                    RaisePropertyChanged("OpenPrice");
                }
            }
        }
        #endregion

        #region TradingDay
        private DateTime _tradingDay;

        public DateTime TradingDay
        {
            get { return _tradingDay; }
            set
            {
                if (_tradingDay != value)
                {
                    _tradingDay = value;
                    RaisePropertyChanged("TradingDay");
                }
            }
        }
        #endregion

        #region ExchangeID
        private string _exchangeID;

        public string ExchangeID
        {
            get { return _exchangeID; }
            set
            {
                if (_exchangeID != value)
                {
                    _exchangeID = value;
                    RaisePropertyChanged("ExchangeID");
                }
            }
        }
        #endregion

        #region CloseProfit
        private double _closeProfit;

        public double CloseProfit
        {
            get { return _closeProfit; }
            set
            {
                if (_closeProfit != value)
                {
                    _closeProfit = value;
                    RaisePropertyChanged("CloseProfit");
                }
            }
        }
        #endregion

        #region PositionProfit
        private double _positionProfit;

        public double PositionProfit
        {
            get { return _positionProfit; }
            set
            {
                if (_positionProfit != value)
                {
                    _positionProfit = value;
                    RaisePropertyChanged("PositionProfit");
                }
            }
        }
        #endregion

        #region Margin
        private double _margine;

        public double Margin
        {
            get { return _margine; }
            set
            {
                if (_margine != value)
                {
                    _margine = value;
                    RaisePropertyChanged("Margin");
                }
            }
        }
        #endregion

        #region MarginRateByMoney
        private double _marginRageByMoney;

        public double MarginRateByMoney
        {
            get { return _marginRageByMoney; }
            set
            {
                if (_marginRageByMoney != value)
                {
                    _marginRageByMoney = value;
                    RaisePropertyChanged("MarginRateByMoney");
                }
            }
        }
        #endregion

        #region CloseVolume
        private int _closeVolume;

        public int CloseVolume
        {
            get { return _closeVolume; }
            set
            {
                if (_closeVolume != value)
                {
                    _closeVolume = value;
                    RaisePropertyChanged("CloseVolume");
                }
            }
        }
        #endregion

        #region CloseAmount
        private double _closeAmount;

        public double CloseAmount
        {
            get { return _closeAmount; }
            set
            {
                if (_closeAmount != value)
                {
                    _closeAmount = value;
                    RaisePropertyChanged("CloseAmount");
                }
            }
        }
        #endregion

    }
}

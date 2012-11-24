using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Commands;
using System.ComponentModel;
using System.Windows.Data;

namespace PortfolioTrading.Modules.Account
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.ComponentModel.Composition;
    using Microsoft.Practices.Prism.ViewModel;
    using System.Collections.ObjectModel;
    using System.Globalization;
    using Microsoft.Practices.Prism.Events;
    using PortfolioTrading.Events;

    namespace HiFreTradeUI.ViewModels
    {
        [Export]
        [PartCreationPolicy(CreationPolicy.NonShared)]
        public class PositionDetailVM : NotificationObject
        {
            private ObservableCollection<PositionDetailItem> _positionDetailItems = new ObservableCollection<PositionDetailItem>();
            private AccountVM _acctVm;

            public PositionDetailVM()
            {
                RefreshPositionCommand = new DelegateCommand(RequestPositionDetail);
                CloseOrderCommand = new DelegateCommand<PositionDetailItem>(OnCloseOrder);

                PositionDetailItems = CollectionViewSource.GetDefaultView(_positionDetailItems);
                PositionDetailItems.Filter = OnFilterPosition;
            }

            public ICollectionView PositionDetailItems
            {
                get;
                private set;
            }

            private bool OnFilterPosition(object item)
            {
                PositionDetailItem posi = item as PositionDetailItem;
                if (posi != null)
                {
                    return IncludingClosed || posi.Volume > 0;
                }

                return false;
            }

            public DelegateCommand RefreshPositionCommand { get; private set; }
            public DelegateCommand<PositionDetailItem> CloseOrderCommand { get; private set; }

            private void RequestPositionDetail()
            {
                if (_acctVm != null)
                {
                    Clear();
                    _acctVm.QueryPositionDetails("");
                }
            }

            private void OnCloseOrder(PositionDetailItem positionItem)
            {
                if (_acctVm != null)
                {
                    if (positionItem.Volume > 0)
                    {
                        _acctVm.ManualCloseOrder(positionItem.Symbol, positionItem.CloseDirection, 
                            positionItem.OffsetFlag, positionItem.Volume);
                    }
                }
            }

            public void Clear()
            {
                _positionDetailItems.Clear();
            }

            public void SetAccount(AccountVM acctVm)
            {
                if (acctVm != null)
                {
                    acctVm.OnPositionDetailReturn += _acctVm_OnPositionDetailReturn;
                }
                else
                {
                    _acctVm.OnPositionDetailReturn -= _acctVm_OnPositionDetailReturn;
                }
                _acctVm = acctVm;
            }

            void _acctVm_OnPositionDetailReturn(trade.PositionDetailInfo posiDetail)
            {
                PositionDetailItem posiItem = new PositionDetailItem();

                posiItem.Symbol = posiDetail.InstrumentID;
                posiItem.TradeID = posiDetail.TradeID;
                posiItem.HedgeFlag = GetHedgeText(posiDetail.HedgeFlag);
                posiItem.Direction = GetDirection(posiDetail.Direction);
                posiItem.OpenDate = GetDate(posiDetail.OpenDate);
                posiItem.Volume = posiDetail.Volume;
                posiItem.IsClosed = posiItem.Volume > 0;
                posiItem.OpenPrice = posiDetail.OpenPrice;
                posiItem.TradingDay = GetDate(posiDetail.TradingDay);
                posiItem.ExchangeID = posiDetail.ExchangeID;
                posiItem.CloseProfit = posiDetail.CloseProfitByDate;
                posiItem.PositionProfit = posiDetail.PositionProfitByDate;
                posiItem.Margin = posiDetail.Margin;
                posiItem.MarginRateByMoney = posiDetail.MarginRateByMoney;
                posiItem.CloseVolume = posiDetail.CloseVolume;
                posiItem.CloseAmount = posiDetail.CloseAmount;

                posiItem.CloseDirection = posiDetail.Direction == trade.TradeDirectionType.BUY ? 
                    trade.TradeDirectionType.SELL : trade.TradeDirectionType.BUY;
                posiItem.OffsetFlag = GetOffsetFlag(posiItem.Symbol, posiItem.OpenDate, posiItem.TradingDay);

                _positionDetailItems.Add(posiItem);

                UpdateTime = DateTime.Now;
            }

            private static trade.OffsetFlagType GetOffsetFlag(string symbol, DateTime openDate, DateTime tradingDay)
            {
                if (symbol.StartsWith("cu", StringComparison.OrdinalIgnoreCase))
                {
                    return openDate < tradingDay ? trade.OffsetFlagType.OF_CLOSE_YESTERDAY : trade.OffsetFlagType.OF_CLOSE_TODAY;
                }
                return trade.OffsetFlagType.OF_CLOSE_TODAY;
            }

            private static string GetHedgeText(trade.HedgeFlagType hedgeFlag)
            {
                ///投机
                ///#define THOST_FTDC_HF_Speculation '1'
                ///套利
                ///#define THOST_FTDC_HF_Arbitrage '2'
                ///套保
                ///#define THOST_FTDC_HF_Hedge '3'
                switch (hedgeFlag)
                {
                    case trade.HedgeFlagType.SPECULATION:
                        return "投机";
                    case trade.HedgeFlagType.ARBITRAGE:
                        return "套利";
                    case trade.HedgeFlagType.HEDGE:
                        return "套保";
                    default:
                        return "";
                }
            }

            private static string GetDirection(trade.TradeDirectionType iDirection)
            {
                switch (iDirection)
                {
                    case trade.TradeDirectionType.SELL:
                        return "卖出";
                    case trade.TradeDirectionType.BUY:
                        return "买入";
                    default:
                        return "未知";
                }
            }

            public static DateTime GetDate(string tradeDate)
            {
                return DateTime.ParseExact(tradeDate, "yyyyMMdd", CultureInfo.CurrentCulture);
            }

            #region UpdateTime
            private DateTime updateTime;

            public DateTime UpdateTime
            {
                get { return updateTime; }
                set
                {
                    if (updateTime != value)
                    {
                        updateTime = value;
                        RaisePropertyChanged("UpdateTime");
                    }
                }
            }
            #endregion

            #region IncludingClosed
            private bool includingClosed;

            public bool IncludingClosed
            {
                get { return includingClosed; }
                set
                {
                    if (includingClosed != value)
                    {
                        includingClosed = value;
                        RaisePropertyChanged("IncludingClosed");
                        PositionDetailItems.Refresh();
                    }
                }
            }
            #endregion
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

            #region TradeID
            private string _tradeID;

            public string TradeID
            {
                get { return _tradeID; }
                set
                {
                    if (_tradeID != value)
                    {
                        _tradeID = value;
                        RaisePropertyChanged("TradeID");
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

            public trade.TradeDirectionType CloseDirection { get; set; }
            public trade.OffsetFlagType OffsetFlag { get; set; }

            #region IsClosed
            private bool _isClosed;

            public bool IsClosed
            {
                get { return _isClosed; }
                set
                {
                    if (_isClosed != value)
                    {
                        _isClosed = value;
                        RaisePropertyChanged("IsClosed");
                    }
                }
            }
            #endregion

        }
    }

}

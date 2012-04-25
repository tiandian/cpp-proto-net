using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using QuickArbitrage.Common.Enums;
using Microsoft.Practices.Prism.ViewModel;

namespace AutoTrade.MainFrame.Modules.Portfolio
{
    public class PortfolioItem : NotificationObject
    {
        private ObservableCollection<LegItem> _legs = new ObservableCollection<LegItem>();

        public IEnumerable<LegItem> Legs
        {
            get { return _legs; }
        }

        #region RunningType
        private TradingType _runningType;

        public TradingType RunningType
        {
            get { return _runningType; }
            set
            {
                if (_runningType != value)
                {
                    _runningType = value;
                    RaisePropertyChanged("RunningType");
                }
            }
        }
        #endregion

        #region LongDiff
        private double? _longDiff;

        public double? LongDiff
        {
            get { return _longDiff; }
            set
            {
                if (_longDiff != value)
                {
                    _longDiff = value;
                    RaisePropertyChanged("LongDiff");
                }
            }
        }
        #endregion

        #region ShortDiff
        private double? _shortDiff;

        public double? ShortDiff
        {
            get { return _shortDiff; }
            set
            {
                if (_shortDiff != value)
                {
                    _shortDiff = value;
                    RaisePropertyChanged("ShortDiff");
                }
            }
        }
        #endregion

        #region Balance
        private double _balance;

        public double Balance
        {
            get { return _balance; }
            set
            {
                if (_balance != value)
                {
                    _balance = value;
                    RaisePropertyChanged("Balance");
                }
            }
        }
        #endregion

        public LegItem CreateLeg()
        {
            return LegItem.Create(this);
        }

        public void AddLeg(LegItem leg)
        {
            _legs.Add(leg);
        }

        public void RemoveLeg(int legIdx)
        {
            if (legIdx < _legs.Count && legIdx >= 0)
                _legs.RemoveAt(legIdx);
        }

        public void RemoveLeg(LegItem leg)
        {
            _legs.Remove(leg);
        }

        public void UpdateDiff()
        {
            if (_legs.Count >= 2)
            {
                LegItem bigger = _legs[0].Last >= _legs[1].Last ? _legs[0] : _legs[1];
                LegItem smaller = _legs[0].Last < _legs[1].Last ? _legs[0] : _legs[1];

                LongDiff = bigger.Ask - smaller.Bid;
                ShortDiff = bigger.Bid - smaller.Ask;
            }
        }

        private string _accountID;
        private string _portfolioIdx;
        public string PID
        {
            get { return _accountID + "_" + _portfolioIdx; }
        }

        private PortfolioItem(string accountId, string pId)
        {
            _accountID = accountId;
            _portfolioIdx = pId;
        }

        internal static PortfolioItem Create(string accountId, string pId)
        {
            return new PortfolioItem(accountId, pId);
        }
    }
}

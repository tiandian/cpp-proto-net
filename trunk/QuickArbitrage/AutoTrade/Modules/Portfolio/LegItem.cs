using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using QuickArbitrage.Common.Enums;

namespace AutoTrade.MainFrame.Modules.Portfolio
{
    public class LegItem : NotificationObject
    {
        private PortfolioItem _parentPortfolio;

        private LegItem(PortfolioItem parent)
        {
            _parentPortfolio = parent;
        }

        internal static LegItem Create(PortfolioItem parent)
        {
            return new LegItem(parent);
        }

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
                    RaisePropertyChanged("Code");
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
                    RaisePropertyChanged("Name");
                }
            }
        }
        #endregion

        #region Last
        private double _last;

        public double Last
        {
            get { return _last; }
            set
            {
                if (_last != value)
                {
                    _last = value;
                    RaisePropertyChanged("Last");
                }
            }
        }
        #endregion

        #region PrevClose
        private double _prevClose;

        public double PrevClose
        {
            get { return _prevClose; }
            set
            {
                if (_prevClose != value)
                {
                    _prevClose = value;
                    RaisePropertyChanged("PrevClose");
                }
            }
        }
        #endregion

        #region Change
        private double _change;

        public double Change
        {
            get { return _change; }
            set
            {
                if (_change != value)
                {
                    _change = value;
                    RaisePropertyChanged("Change");
                }
            }
        }
        #endregion

        #region PercentChg
        private double _percentChg;

        public double PercentChg
        {
            get { return _percentChg; }
            set
            {
                if (_percentChg != value)
                {
                    _percentChg = value;
                    RaisePropertyChanged("PercentChg");
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
                    RaisePropertyChanged("Quantity");
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
                    RaisePropertyChanged("Side");
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
                    RaisePropertyChanged("Ask");
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
                    RaisePropertyChanged("Bid");
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
                    RaisePropertyChanged("LegStatus");
                }
            }
        }
        #endregion
    }
}

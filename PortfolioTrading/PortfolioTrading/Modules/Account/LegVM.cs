﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using entity;

namespace PortfolioTrading.Modules.Account
{
    public class LegVM : NotificationObject
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

        #region Ratio
        private int _ratio;

        public int Ratio
        {
            get { return _ratio; }
            set
            {
                if (_ratio != value)
                {
                    _ratio = value;
                    RaisePropertyChanged("Ratio");
                }
            }
        }
        #endregion

        #region Side
        private PosiDirectionType _side;

        public PosiDirectionType Side
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

        #region Status
        private LegStatus _status;

        public LegStatus Status
        {
            get { return _status; }
            set
            {
                if (_status != value)
                {
                    _status = value;
                    RaisePropertyChanged("Status");
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

    }

}
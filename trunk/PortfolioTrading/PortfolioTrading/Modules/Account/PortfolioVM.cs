using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;

namespace PortfolioTrading.Modules.Account
{
    public class PortfolioVM : NotificationObject
    {
        private ObservableCollection<LegVM> _legs = new ObservableCollection<LegVM>();

        #region Id
        private string _id;

        public string Id
        {
            get { return _id; }
            set
            {
                if (_id != value)
                {
                    _id = value;
                    RaisePropertyChanged("Id");
                }
            }
        }
        #endregion

        #region Quantity
        private int _qty;

        public int Quantity
        {
            get { return _qty; }
            set
            {
                if (_qty != value)
                {
                    _qty = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

        #region Diff
        private double _diff;

        public double Diff
        {
            get { return _diff; }
            set
            {
                if (_diff != value)
                {
                    _diff = value;
                    RaisePropertyChanged("Diff");
                }
            }
        }
        #endregion

        #region AutoOpen
        private bool _autoOpen;

        public bool AutoOpen
        {
            get { return _autoOpen; }
            set
            {
                if (_autoOpen != value)
                {
                    _autoOpen = value;
                    RaisePropertyChanged("AutoOpen");
                }
            }
        }
        #endregion

        #region AutoClose
        private bool _autoClose;

        public bool AutoClose
        {
            get { return _autoClose; }
            set
            {
                if (_autoClose != value)
                {
                    _autoClose = value;
                    RaisePropertyChanged("AutoClose");
                }
            }
        }
        #endregion

        public IEnumerable<LegVM> Legs
        {
            get { return _legs; }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using QuickArbitrage.Main.Util;
using QuickArbitrage.Common.Enums;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace QuickArbitrage.Main.ViewModel
{
    class RunningPortfolioItem : BaseViewModel
    {
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
        			this.OnPropertyChanged("RunningType");
        		}
        	}
        }
        #endregion

        #region Strategy
        
        private Strategy _strategy;
        public Strategy Strategy
        {
        	get { return _strategy; }
        	set
        	{
        		if (_strategy != value)
        		{
        			_strategy = value;
        			this.OnPropertyChanged("Strategy");
        		}
        	}
        }
        #endregion

        private ObservableCollection<EquityItem> _equityItems;
        public IList<EquityItem> Equities
        {
            get { return _equityItems; }
        }

        #region Diff
        
        private float _diff;
        public float Diff
        {
        	get { return _diff; }
        	set
        	{
                if (!_diff.equals(value))
        		{
        			_diff = value;
        			this.OnPropertyChanged("Diff");
        		}
        	}
        }
        #endregion

        #region Balance
        
        private float _Balance;
        public float Balance
        {
        	get { return _Balance; }
        	set
        	{
                if (!_Balance.equals(value))
        		{
        			_Balance = value;
        			this.OnPropertyChanged("Balance");
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

        public RunningPortfolioItem()
        {
            _equityItems = new ObservableCollection<EquityItem>();
            _equityItems.CollectionChanged += new NotifyCollectionChangedEventHandler(_equityItems_CollectionChanged);
        }

        void _equityItems_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            switch (e.Action)
            {
                case NotifyCollectionChangedAction.Add:
                    foreach (var item in e.NewItems)
                    {
                        INotifyPropertyChanged propNotifier = item as INotifyPropertyChanged;
                        if (propNotifier != null)
                        {
                            propNotifier.PropertyChanged += propNotifier_PropertyChanged;
                        }
                    }
                    break;
                case NotifyCollectionChangedAction.Remove:
                    if (e.OldItems != null)
                    {
                        foreach (var item in e.OldItems)
                        {
                            INotifyPropertyChanged propNotifier = item as INotifyPropertyChanged;
                            if (propNotifier != null)
                            {
                                propNotifier.PropertyChanged -= propNotifier_PropertyChanged;
                            }
                        }
                    }
                    break;
                case NotifyCollectionChangedAction.Reset:
                    foreach (var item in _equityItems)
                    {
                        INotifyPropertyChanged propNotifier = item as INotifyPropertyChanged;
                        if (propNotifier != null)
                        {
                            propNotifier.PropertyChanged -= propNotifier_PropertyChanged;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        void propNotifier_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Last")
            {
                UpdateDiff();
                UpdateBalance();
            }
        }

        public void UpdateDiff()
        {
            if (_equityItems.Count >= 2)
            {
                Diff = _equityItems[0].Last - _equityItems[1].Last;
            }
        }

        public void UpdateBalance()
        {
            if (_equityItems.Count >= 2)
            {
                Balance = _equityItems[0].Gain + _equityItems[1].Gain;
            }
        }
    }
}

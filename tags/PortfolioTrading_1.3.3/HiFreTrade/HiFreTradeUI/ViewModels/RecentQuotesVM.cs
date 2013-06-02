using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using Microsoft.Practices.ServiceLocation;
using Microsoft.Practices.Prism.Events;
using HiFreTradeUI.Events;
using Microsoft.Practices.Prism.Commands;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class RecentQuotesVM : NotificationObject
    {
        private ObservableCollection<QuoteItem> _recentQuotes = new ObservableCollection<QuoteItem>();

        public RecentQuotesVM()
        {
            HistorySize = 30;
            ClearCommand = new DelegateCommand(OnClearCommandExecuted);

            IEventAggregator evtAgg = ServiceLocator.Current.GetInstance<IEventAggregator>();
            evtAgg.GetEvent<QuoteUpdateEvent>().Subscribe(OnQuoteUpdated, ThreadOption.UIThread, true);
        }

        public DelegateCommand ClearCommand { get; private set; }

        public int HistorySize { get; set; }

        #region Pause
        private bool _pause = false;

        public bool Pause
        {
            get { return _pause; }
            set
            {
                if (_pause != value)
                {
                    _pause = value;
                    RaisePropertyChanged("Pause");
                }
            }
        }
        #endregion

        public void Clear()
        {
            _recentQuotes.Clear();
        }

        private void OnClearCommandExecuted()
        {
            Clear();
        }

        public IEnumerable<QuoteItem> Quotes
        {
            get { return _recentQuotes; }
        }

        public void OnQuoteUpdated(QuoteItem quoteItem)
        {
            if (Pause) return;

            // insert at header
            _recentQuotes.Insert(0, quoteItem);

            if (_recentQuotes.Count > HistorySize)
            {
                // remove from tailer
                _recentQuotes.RemoveAt(HistorySize);
            }
        }
    }

    public class QuoteItem : NotificationObject
    {
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

        #region UpdateTime
        private DateTime _updateTime;

        public DateTime UpdateTime
        {
            get { return _updateTime; }
            set
            {
                if (_updateTime != value)
                {
                    _updateTime = value;
                    RaisePropertyChanged("UpdateTime");
                }
            }
        }
        #endregion

    }
}

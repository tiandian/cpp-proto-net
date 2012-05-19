using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using HiFreTradeUI.ViewModels;

namespace HiFreTradeUI
{
    [Export]
    public class ShellViewModel : NotificationObject
    {
        private ObservableCollection<string> _availableSymbols = new ObservableCollection<string>();
        private ObservableCollection<EntryTypeItem> _entryTypeItemSource = new ObservableCollection<EntryTypeItem>();

        public ShellViewModel()
        {
            _availableSymbols.Add("IF1205");
            _availableSymbols.Add("IF1206");
            _availableSymbols.Add("IF1209");
            _availableSymbols.Add("IF1212");

            _entryTypeItemSource.Add(new EntryTypeItem() { EntryID = 2, DisplayName = "多开" });
            _entryTypeItemSource.Add(new EntryTypeItem() { EntryID = 1, DisplayName = "空开" });
        }

        [Import]
        private UIThread UIThread { get; set; }

        #region IsStarted
        private bool _isStarted = false;

        public bool IsStarted
        {
            get { return _isStarted; }
            set
            {
                if (_isStarted != value)
                {
                    _isStarted = value;

                    if (_isStarted)
                        Start();
                    else
                        Stop();

                    RaisePropertyChanged("IsStarted");
                    RaisePropertyChanged("IsStop");
                }
            }
        }
        #endregion

        #region IsStop
        public bool IsStop
        {
            get { return !_isStarted; }
            set
            {
                IsStarted = !value;
            }
        }
        #endregion

        #region ManualEntryType
        private int _manualEntryType;

        public int ManualEntryType
        {
            get { return _manualEntryType; }
            set
            {
                if (_manualEntryType != value)
                {
                    _manualEntryType = value;
                    RaisePropertyChanged("ManualEntryType");
                }
            }
        }
        #endregion


        [Import]
        public QuoteVM Quote { get; set; }

        [Import]
        public TradeVM Trade { get; set; }

        public IEnumerable<string> AvaialbeSymbols
        {
            get
            {
                return _availableSymbols;
            }
        }

        public IEnumerable<EntryTypeItem> EntryTypeItemSource
        {
            get { return _entryTypeItemSource; }
        }

        public void Connect()
        {
            Quote.Connect();

            //Trade.Connect();
        }

        public void Start()
        {
            Trade.Start();
        }

        public void Stop()
        {
            Trade.Stop();
        }

        public void Disconnect()
        {
            Trade.Disconnect();
            Quote.Disconnect();
        }

        public class EntryTypeItem
        {
            public int EntryID { get; set; }
            public string DisplayName { get; set; }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.ServiceLocation;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.Prism.ViewModel;
using HiFreTradeUI.Events;
using System.Collections.ObjectModel;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class OperationsVM
    {
        private ObservableCollection<OperationRecord> _records = new ObservableCollection<OperationRecord>();

        public OperationsVM()
        {
            IEventAggregator evntAggr = ServiceLocator.Current.GetInstance<IEventAggregator>();
            evntAggr.GetEvent<OperRecordUpdateEvent>().Subscribe(UpdateOperationRecord, ThreadOption.UIThread, true);
        }

        public void UpdateOperationRecord(OperationRecord record)
        {
            OperationRecord existingRec = _records.FirstOrDefault(r => r.RecordID == record.RecordID);
            if (existingRec != null)
            {
                // update
            }
            else
            {
                // add new one
                _records.Add(record);
            }
        }

        public IEnumerable<OperationRecord> Records { get { return _records; } }
    }

    public class OperationRecord : NotificationObject
    {
        #region RecordID
        private int _recordID;

        public int RecordID
        {
            get { return _recordID; }
            set
            {
                if (_recordID != value)
                {
                    _recordID = value;
                    RaisePropertyChanged("RecordID");
                }
            }
        }
        #endregion

    }
}

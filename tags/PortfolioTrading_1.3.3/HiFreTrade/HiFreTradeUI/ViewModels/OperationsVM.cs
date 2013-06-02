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
                existingRec.Symbol = record.Symbol;

                existingRec.RectPeriodBegin = record.RectPeriodBegin;
                existingRec.RectPeriodEnd = record.RectPeriodEnd;
                existingRec.UpperBoundary = record.UpperBoundary;
                existingRec.LowerBoundary = record.LowerBoundary;
                existingRec.Range = record.Range;
                existingRec.Direction = record.Direction;

                existingRec.EntryPoint = record.EntryPoint;
                existingRec.EntryTime = record.EntryTime;
                existingRec.EntryReason = record.EntryReason;
                existingRec.EntryType = record.EntryType;
                existingRec.EntryQuantity = record.EntryQuantity;
                existingRec.EntryStatus = record.EntryStatus;

                existingRec.ExitPoint = record.ExitPoint;
                existingRec.ExitTime = record.ExitTime;
                existingRec.ExitReason = record.ExitReason;
                existingRec.ExitType = record.ExitType;
                existingRec.ExitQuantity = record.ExitQuantity;
                existingRec.ExitStatus = record.ExitStatus;

                existingRec.ProfitLoss = record.ProfitLoss;
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

        #region RectPeriodBegin
        private DateTime _rectPeriodBegin;

        public DateTime RectPeriodBegin
        {
            get { return _rectPeriodBegin; }
            set
            {
                if (_rectPeriodBegin != value)
                {
                    _rectPeriodBegin = value;
                    RaisePropertyChanged("RectPeriodBegin");
                }
            }
        }
        #endregion

        #region RectPeriodEnd
        private DateTime _rectPeriodEnd;

        public DateTime RectPeriodEnd
        {
            get { return _rectPeriodEnd; }
            set
            {
                if (_rectPeriodEnd != value)
                {
                    _rectPeriodEnd = value;
                    RaisePropertyChanged("RectPeriodEnd");
                }
            }
        }
        #endregion

        #region UpperBoundary
        private double _upperBoundary;

        public double UpperBoundary
        {
            get { return _upperBoundary; }
            set
            {
                if (_upperBoundary != value)
                {
                    _upperBoundary = value;
                    RaisePropertyChanged("UpperBoundary");
                }
            }
        }
        #endregion

        #region LowerBoundary
        private double _lowerBoundary;

        public double LowerBoundary
        {
            get { return _lowerBoundary; }
            set
            {
                if (_lowerBoundary != value)
                {
                    _lowerBoundary = value;
                    RaisePropertyChanged("LowerBoundary");
                }
            }
        }
        #endregion

        #region Range
        private double _range;

        public double Range
        {
            get { return _range; }
            set
            {
                if (_range != value)
                {
                    _range = value;
                    RaisePropertyChanged("Range");
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

        #region EntryPoint
        private double _entryPoint;

        public double EntryPoint
        {
            get { return _entryPoint; }
            set
            {
                if (_entryPoint != value)
                {
                    _entryPoint = value;
                    RaisePropertyChanged("EntryPoint");
                }
            }
        }
        #endregion

        #region EntryTime
        private DateTime _entryTime;

        public DateTime EntryTime
        {
            get { return _entryTime; }
            set
            {
                if (_entryTime != value)
                {
                    _entryTime = value;
                    RaisePropertyChanged("EntryTime");
                }
            }
        }
        #endregion

        #region EntryReason
        private string _entryReason;

        public string EntryReason
        {
            get { return _entryReason; }
            set
            {
                if (_entryReason != value)
                {
                    _entryReason = value;
                    RaisePropertyChanged("EntryReason");
                }
            }
        }
        #endregion

        #region EntryType
        private string _entryType;

        public string EntryType
        {
            get { return _entryType; }
            set
            {
                if (_entryType != value)
                {
                    _entryType = value;
                    RaisePropertyChanged("EntryType");
                }
            }
        }
        #endregion

        #region EntryQuantity
        private int _entryQuantity;

        public int EntryQuantity
        {
            get { return _entryQuantity; }
            set
            {
                if (_entryQuantity != value)
                {
                    _entryQuantity = value;
                    RaisePropertyChanged("EntryQuantity");
                }
            }
        }
        #endregion

        #region EntryStatus
        private string _entryStatus;

        public string EntryStatus
        {
            get { return _entryStatus; }
            set
            {
                if (_entryStatus != value)
                {
                    _entryStatus = value;
                    RaisePropertyChanged("EntryStatus");
                }
            }
        }
        #endregion

        #region ExitPoint
        private double _exitPoint;

        public double ExitPoint
        {
            get { return _exitPoint; }
            set
            {
                if (_exitPoint != value)
                {
                    _exitPoint = value;
                    RaisePropertyChanged("ExitPoint");
                }
            }
        }
        #endregion

        #region ExitTime
        private DateTime _exitTime;

        public DateTime ExitTime
        {
            get { return _exitTime; }
            set
            {
                if (_exitTime != value)
                {
                    _exitTime = value;
                    RaisePropertyChanged("ExitTime");
                }
            }
        }
        #endregion

        #region ExitReason
        private string _exitReason;

        public string ExitReason
        {
            get { return _exitReason; }
            set
            {
                if (_exitReason != value)
                {
                    _exitReason = value;
                    RaisePropertyChanged("ExitReason");
                }
            }
        }
        #endregion

        #region ExitType
        private string _exitType;

        public string ExitType
        {
            get { return _exitType; }
            set
            {
                if (_exitType != value)
                {
                    _exitType = value;
                    RaisePropertyChanged("ExitType");
                }
            }
        }
        #endregion

        #region ExitQuantity
        private int _exitQuantity;

        public int ExitQuantity
        {
            get { return _exitQuantity; }
            set
            {
                if (_exitQuantity != value)
                {
                    _exitQuantity = value;
                    RaisePropertyChanged("ExitQuantity");
                }
            }
        }
        #endregion

        #region ExitStatus
        private string _exitStatus;

        public string ExitStatus
        {
            get { return _exitStatus; }
            set
            {
                if (_exitStatus != value)
                {
                    _exitStatus = value;
                    RaisePropertyChanged("ExitStatus");
                }
            }
        }
        #endregion

        #region ProfitLoss
        private double _profitLoss;

        public double ProfitLoss
        {
            get { return _profitLoss; }
            set
            {
                if (_profitLoss != value)
                {
                    _profitLoss = value;
                    RaisePropertyChanged("ProfitLoss");
                }
            }
        }
        #endregion

    }
}

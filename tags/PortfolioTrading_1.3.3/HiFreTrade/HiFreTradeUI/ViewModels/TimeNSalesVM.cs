using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.ServiceLocation;
using HiFreTradeUI.Events;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class TimeNSalesVM
    {
        private ObservableCollection<TimeNSalesData> _timeNSalesList = new ObservableCollection<TimeNSalesData>();

        public TimeNSalesVM()
        {
            IEventAggregator evntAggr = ServiceLocator.Current.GetInstance<IEventAggregator>();
            evntAggr.GetEvent<TimeNSalesUpdateEvent>().Subscribe(UpdateTimeNSales, ThreadOption.UIThread, true);
        }

        public IEnumerable<TimeNSalesData> TimeNSalesList
        {
            get { return _timeNSalesList; }
        }

        public void UpdateTimeNSales(TimeNSalesData tnsData)
        {
            _timeNSalesList.Add(tnsData);
        }
    }

    public class TimeNSalesData : NotificationObject
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

        #region TimeStamp
        private DateTime _timeStamp;

        public DateTime TimeStamp
        {
            get { return _timeStamp; }
            set
            {
                if (_timeStamp != value)
                {
                    _timeStamp = value;
                    RaisePropertyChanged("TimeStamp");
                }
            }
        }
        #endregion

        #region Price
        private double _price;

        public double Price
        {
            get { return _price; }
            set
            {
                if (_price != value)
                {
                    _price = value;
                    RaisePropertyChanged("Price");
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

    }
}

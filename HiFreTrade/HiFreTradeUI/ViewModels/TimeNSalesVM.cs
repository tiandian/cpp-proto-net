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

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using Microsoft.Practices.Prism.ViewModel;

namespace PortfolioTrading.Utils
{
    public class SideBindingSource
    {
        private ObservableCollection<SideItem> _itemsSource = new ObservableCollection<SideItem>();

        public IEnumerable<SideItem> Items { get { return _itemsSource; } }

        public SideBindingSource()
        {
            _itemsSource.Add(new SideItem() { Side = PTEntity.PosiDirectionType.LONG, DisplayText = "多头" });
            _itemsSource.Add(new SideItem() { Side = PTEntity.PosiDirectionType.SHORT, DisplayText = "空头" });
        }
    }

    public class SideItem : NotificationObject
    {
        #region Side
        private PTEntity.PosiDirectionType _side;

        public PTEntity.PosiDirectionType Side
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

        #region DisplayText
        private string _displayText;

        public string DisplayText
        {
            get { return _displayText; }
            set
            {
                if (_displayText != value)
                {
                    _displayText = value;
                    RaisePropertyChanged("DisplayText");
                }
            }
        }
        #endregion
    }
}

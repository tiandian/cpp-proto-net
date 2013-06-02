using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace QuickArbitrage.Main.Util
{
    public abstract class BaseViewModel : INotifyPropertyChanged
        //, IDataErrorInfo
    {
        public BaseViewModel()
        {
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = this.PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));

        }

        /*
        #region IDataErrorInfo Members
        public ValidationHandler validationHandler = new ValidationHandler();
        public string Error
        {
            get { return null; }
        }
        public string this[string columnName]
        {
            get
            {
                if (this.validationHandler.BrokenRuleExists(columnName))
                {
                    return this.validationHandler[columnName];
                }
                return null;
            }
        }
        #endregion
        */
    }
}

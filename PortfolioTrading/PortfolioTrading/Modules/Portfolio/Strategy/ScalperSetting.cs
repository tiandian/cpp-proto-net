using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Xml.Linq;
using APInvokeManaged;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ScalperSetting : StrategySetting, INotifyPropertyChanged
    {
        #region Threshold
        private double _diff;

        public double Threshold
        {
            get { return _diff; }
            set
            {
                if (_diff != value)
                {
                    _diff = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion


        public override string Name
        {
            get { return ScalperStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("scalperStrategySetting",
                new XAttribute("threshold", Threshold));

            return elem.ToString();
        }

        public override byte[] Serialize()
        {
            entity.ScalperSettings setting = new entity.ScalperSettings();
            setting.Threshold = Threshold;
            return DataTranslater.Serialize(setting);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string propName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Xml.Linq;
using APInvokeManaged;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ChangePositionSetting : StrategySetting, INotifyPropertyChanged
    {
        #region CloseLeg
        private string _closeLeg;

        public string CloseLeg
        {
            get { return _closeLeg; }
            set
            {
                if (_closeLeg != value)
                {
                    _closeLeg = value;
                    RaisePropertyChanged("CloseLeg");
                }
            }
        }
        #endregion

        #region TriggerCondition
        private CompareCondition _triggerCond;

        public CompareCondition TriggerCondition
        {
            get { return _triggerCond; }
            set
            {
                if (_triggerCond != value)
                {
                    _triggerCond = value;
                    RaisePropertyChanged("TriggerCondition");
                }
            }
        }
        #endregion

        #region Threshold
        private double _threshold;

        public double Threshold
        {
            get { return _threshold; }
            set
            {
                if (_threshold != value)
                {
                    _threshold = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion


        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string propName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }

        public override string Name
        {
            get { return StrategySetting.ChangePositionStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("changePositionStrategySetting");
            elem.Add(new XAttribute("closeLeg", CloseLeg));
            elem.Add(new XAttribute("triggerCondition", TriggerCondition.ToString()));
            elem.Add(new XAttribute("threshold", Threshold));
            
            return elem.ToString();
        }

        public override byte[] Serialize()
        {
            entity.ChangePosiStrategySettings settings = new entity.ChangePosiStrategySettings();
            settings.CloseLeg = CloseLeg;
            settings.TriggerCondition = (entity.CompareCondition)TriggerCondition;
            settings.Threshold = Threshold;

            return DataTranslater.Serialize<entity.ChangePosiStrategySettings>(settings);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using APInvokeManaged;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public enum CompareCondition
    {
        GREATER_THAN,
        GREATER_EQUAL_THAN,
        LESS_THAN,
        LESS_EQUAL_THAN
    }

    public class ArbitrageStrategySetting : StrategySetting, INotifyPropertyChanged
    {
        #region Direction
        private entity.PosiDirectionType _direction;

        public entity.PosiDirectionType Direction
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

        #region OpenCondition
        private CompareCondition _openCond;

        public CompareCondition OpenCondition
        {
            get { return _openCond; }
            set
            {
                if (_openCond != value)
                {
                    _openCond = value;
                    RaisePropertyChanged("OpenCondition");
                }
            }
        }
        #endregion

        #region OpenThreshold
        private double _openThreshold;

        public double OpenThreshold
        {
            get { return _openThreshold; }
            set
            {
                if (_openThreshold != value)
                {
                    _openThreshold = value;
                    RaisePropertyChanged("OpenThreshold");
                }
            }
        }
        #endregion

        #region StopGainCondition
        private CompareCondition _stopGainCond;

        public CompareCondition StopGainCondition
        {
            get { return _stopGainCond; }
            set
            {
                if (_stopGainCond != value)
                {
                    _stopGainCond = value;
                    RaisePropertyChanged("StopGainCondition");
                }
            }
        }
        #endregion

        #region StopGainThreshold
        private double _stopGainThreshold;

        public double StopGainThreshold
        {
            get { return _stopGainThreshold; }
            set
            {
                if (_stopGainThreshold != value)
                {
                    _stopGainThreshold = value;
                    RaisePropertyChanged("StopGainThreshold");
                }
            }
        }
        #endregion

        #region StopLossCondition
        private CompareCondition _stopLossCond;

        public CompareCondition StopLossCondition
        {
            get { return _stopLossCond; }
            set
            {
                if (_stopLossCond != value)
                {
                    _stopLossCond = value;
                    RaisePropertyChanged("StopLossCondition");
                }
            }
        }
        #endregion

        #region StopLossThreshold
        private double _stopLossThreshold;

        public double StopLossThreshold
        {
            get { return _stopLossThreshold; }
            set
            {
                if (_stopLossThreshold != value)
                {
                    _stopLossThreshold = value;
                    RaisePropertyChanged("StopLossThreshold");
                }
            }
        }
        #endregion

        public override string Persist()
        {
            XElement elem = new XElement("arbitrageStrategySetting");
            elem.Add(new XAttribute("direction", Direction.ToString()));
            XElement elemOpen = new XElement("open",
                new XAttribute("condition", OpenCondition),
                new XAttribute("threshold", OpenThreshold));
            elem.Add(elemOpen);
            XElement elemStopGain = new XElement("stopGain",
                new XAttribute("condition", StopGainCondition),
                new XAttribute("threshold", StopGainThreshold));
            elem.Add(elemStopGain);
            XElement elemStopLoss = new XElement("stopLoss",
                new XAttribute("condition", StopLossCondition),
                new XAttribute("threshold", StopLossThreshold));
            elem.Add(elemStopLoss);
            return elem.ToString();
        }

        public override string Name
        {
            get { return ArbitrageStrategyName; }
        }

        public override byte[] Serialize()
        {
            entity.ArbitrageStrategySettings entitySettings = new entity.ArbitrageStrategySettings();
            entitySettings.Side = Direction;
            entitySettings.OpenCondition = (entity.CompareCondition)OpenCondition;
            entitySettings.OpenPosiThreshold = OpenThreshold;
            entitySettings.StopGainCondition = (entity.CompareCondition)StopGainCondition;
            entitySettings.StopGainThreshold = StopGainThreshold;
            entitySettings.StopLossCondition = (entity.CompareCondition)StopLossCondition;
            entitySettings.StopLossThreshold = StopLossThreshold;
            return DataTranslater.Serialize(entitySettings);
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

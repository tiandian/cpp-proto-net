using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ManualStrategySetting : StrategySetting
    {
        public override string Name
        {
            get { return ManualStrategyName; }
        }

        #region RetryTimes
        private int _retryTimes = 8;

        public int RetryTimes
        {
            get { return _retryTimes; }
            set
            {
                if (_retryTimes != value)
                {
                    _retryTimes = value;
                    RaisePropertyChanged("RetryTimes");
                }
            }
        }
        #endregion

        #region StopGainCondition
        private PTEntity.CompareCondition _stopGainCond;

        public PTEntity.CompareCondition StopGainCondition
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
        private PTEntity.CompareCondition _stopLossCond;

        public PTEntity.CompareCondition StopLossCondition
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

        public ManualStrategySetting()
        {
            StopGainCondition = PTEntity.CompareCondition.GREATER_THAN;
            StopGainThreshold = 10;
            StopLossCondition = PTEntity.CompareCondition.GREATER_THAN;
            StopLossThreshold = 6;
        }

        public override string Persist()
        {
            XElement elem = new XElement("manualSetting",
                new XAttribute("retryTimes", RetryTimes));
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

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("retryTimes");
            
            if (attr != null)
            {
                RetryTimes = int.Parse(attr.Value);
            }

            XElement elemStopGain = elem.Element("stopGain");
            attr = elemStopGain.Attribute("condition");
            if (attr != null)
            {
                StopGainCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemStopGain.Attribute("threshold");
            if (attr != null)
            {
                StopGainThreshold = double.Parse(attr.Value);
            }

            XElement elemStopLoss = elem.Element("stopLoss");
            attr = elemStopLoss.Attribute("condition");
            if (attr != null)
            {
                StopLossCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemStopLoss.Attribute("threshold");
            if (attr != null)
            {
                StopLossThreshold = double.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ManualStrategyItem manualStrategy = new PTEntity.ManualStrategyItem();
            manualStrategy.RetryTimes = RetryTimes;

            manualStrategy.StopGainCondition = StopGainCondition;
            manualStrategy.StopGainThreshold = StopGainThreshold;
            manualStrategy.StopLossCondition = StopLossCondition;
            manualStrategy.StopLossThreshold = StopLossThreshold;
            
            return manualStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ManualStrategySetting strategySetting = (ManualStrategySetting)settings;
            RetryTimes = strategySetting.RetryTimes;
        }
    }
}

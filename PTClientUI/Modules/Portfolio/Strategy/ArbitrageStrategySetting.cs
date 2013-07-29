using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Microsoft.Practices.Prism.ViewModel;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ArbitrageStrategySetting : StrategySetting
    {
        #region Direction
        private PTEntity.PosiDirectionType _direction;

        public PTEntity.PosiDirectionType Direction
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
        private PTEntity.CompareCondition _openCond;

        public PTEntity.CompareCondition OpenCondition
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

        public ArbitrageStrategySetting()
        {
            Direction = PTEntity.PosiDirectionType.LONG;
            OpenCondition = PTEntity.CompareCondition.LESS_EQUAL_THAN;
            OpenThreshold = 0;
            StopGainCondition = PTEntity.CompareCondition.GREATER_THAN;
            StopGainThreshold = 0;
            StopLossCondition = PTEntity.CompareCondition.GREATER_THAN;
            StopLossThreshold = 0;
        }

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

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);

            XAttribute attr = elem.Attribute("direction");
            if (attr != null)
                Direction = (PTEntity.PosiDirectionType)Enum.Parse(typeof(PTEntity.PosiDirectionType), attr.Value);

            XElement elemOpen = elem.Element("open");
            attr = elemOpen.Attribute("condition");
            if (attr != null)
            {
                OpenCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemOpen.Attribute("threshold");
            if (attr != null)
            {
                OpenThreshold = double.Parse(attr.Value);
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

        public override string Name
        {
            get { return ArbitrageStrategyName; }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ArbitrageStrategyItem strategyItem = new PTEntity.ArbitrageStrategyItem();
            strategyItem.OpenTimeout = 200;
            strategyItem.RetryTimes = 0;

            PTEntity.ArbitrageTriggerItem openTrigger = new PTEntity.ArbitrageTriggerItem()
            {
                Condition = OpenCondition,
                Offset = PTEntity.PosiOffsetFlag.OPEN,
                Threshold = OpenThreshold,
                Enabled = true
            };
            strategyItem.Triggers.Add(openTrigger);
            return strategyItem;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ArbitrageStrategySetting strategySettings = (ArbitrageStrategySetting)settings;
            this.Direction = strategySettings.Direction;
            this.OpenCondition = strategySettings.OpenCondition;
            this.OpenThreshold = strategySettings.OpenThreshold;
            this.StopGainCondition = strategySettings.StopGainCondition;
            this.StopGainThreshold = strategySettings.StopGainThreshold;
            this.StopLossCondition = strategySettings.StopLossCondition;
            this.StopLossThreshold = strategySettings.StopLossThreshold;
        }
    }
}

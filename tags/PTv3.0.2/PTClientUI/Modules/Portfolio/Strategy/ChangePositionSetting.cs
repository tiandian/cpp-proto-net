using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ChangePositionSetting : StrategySetting
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

        #region CloseLegSide
        private PTEntity.PosiDirectionType _closeLegSide;

        public PTEntity.PosiDirectionType CloseLegSide
        {
            get { return _closeLegSide; }
            set
            {
                if (_closeLegSide != value)
                {
                    _closeLegSide = value;
                    RaisePropertyChanged("CloseLegSide");
                }
            }
        }
        #endregion

        #region TriggerCondition
        private PTEntity.CompareCondition _triggerCond;

        public PTEntity.CompareCondition TriggerCondition
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

        public ChangePositionSetting()
        {
            TriggerCondition = PTEntity.CompareCondition.GREATER_EQUAL_THAN;
            Threshold = 100;
        }

        public override string Name
        {
            get { return StrategySetting.ChangePositionStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("changePositionStrategySetting");
            elem.Add(new XAttribute("closeLeg", CloseLeg));
            elem.Add(new XAttribute("side", CloseLegSide.ToString()));
            elem.Add(new XAttribute("triggerCondition", TriggerCondition.ToString()));
            elem.Add(new XAttribute("threshold", Threshold));
            
            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);

            XAttribute attr = elem.Attribute("closeLeg");
            if (attr != null)
                CloseLeg = attr.Value;

            attr = elem.Attribute("side");
            if (attr != null)
                CloseLegSide = (PTEntity.PosiDirectionType)Enum.Parse(typeof(PTEntity.PosiDirectionType), attr.Value);

            attr = elem.Attribute("triggerCondition");
            if (attr != null)
            {
                TriggerCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }

            attr = elem.Attribute("threshold");
            if (attr != null)
            {
                Threshold = double.Parse(attr.Value);
            }            
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ChangePositionStrategyItem strategyItem = new PTEntity.ChangePositionStrategyItem();
            strategyItem.OpenTimeout = 200;
            strategyItem.RetryTimes = 0;
            /* TODO
            settings.CloseLeg = CloseLeg;
            settings.CloseLegSide = (entity.PosiDirectionType)CloseLegSide;
            */
            PTEntity.ChangePositionTriggerItem openTrigger = new PTEntity.ChangePositionTriggerItem()
            {
                Condition = TriggerCondition,
                Threshold = Threshold,
                Enabled = true
            };
            strategyItem.Triggers.Add(openTrigger);
            return strategyItem;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ChangePositionSetting strategySettings = (ChangePositionSetting)settings;
            this.CloseLeg = strategySettings.CloseLeg;
            this.CloseLegSide = strategySettings.CloseLegSide;
            this.TriggerCondition = strategySettings.TriggerCondition;
            this.Threshold = strategySettings.Threshold;
        }
    }
}

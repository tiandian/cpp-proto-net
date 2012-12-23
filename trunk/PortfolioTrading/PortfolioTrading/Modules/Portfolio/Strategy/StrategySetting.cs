using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public abstract class StrategySetting
    {
        public const string ArbitrageStrategyName = "ArbitrageStrategy";
        public const string ChangePositionStrategyName = "ChangePosition";

        public abstract string Name { get; }

        public abstract string Persist();

        public abstract byte[] Serialize();

        public static StrategySetting Load(string name, string xmlText)
        {
            if (name == ArbitrageStrategyName)
            {
                ArbitrageStrategySetting setting = new ArbitrageStrategySetting();

                XElement elem = XElement.Parse(xmlText);

                XAttribute attr = elem.Attribute("direction");
                if (attr != null)
                    setting.Direction = (entity.PosiDirectionType)Enum.Parse(typeof(entity.PosiDirectionType), attr.Value);

                XElement elemOpen = elem.Element("open");
                attr = elemOpen.Attribute("condition");
                if (attr != null)
                {
                    setting.OpenCondition = (CompareCondition)Enum.Parse(typeof(CompareCondition), attr.Value);
                }
                attr = elemOpen.Attribute("threshold");
                if (attr != null)
                {
                    setting.OpenThreshold = double.Parse(attr.Value);
                }

                XElement elemStopGain = elem.Element("stopGain");
                attr = elemStopGain.Attribute("condition");
                if (attr != null)
                {
                    setting.StopGainCondition = (CompareCondition)Enum.Parse(typeof(CompareCondition), attr.Value);
                }
                attr = elemStopGain.Attribute("threshold");
                if (attr != null)
                {
                    setting.StopGainThreshold = double.Parse(attr.Value);
                }

                XElement elemStopLoss = elem.Element("stopLoss");
                attr = elemStopLoss.Attribute("condition");
                if (attr != null)
                {
                    setting.StopLossCondition = (CompareCondition)Enum.Parse(typeof(CompareCondition), attr.Value);
                }
                attr = elemStopLoss.Attribute("threshold");
                if (attr != null)
                {
                    setting.StopLossThreshold = double.Parse(attr.Value);
                }

                return setting;
            }
            else if (name == ChangePositionStrategyName)
            {
                ChangePositionSetting setting = new ChangePositionSetting();

                XElement elem = XElement.Parse(xmlText);

                XAttribute attr = elem.Attribute("closeLeg");
                if (attr != null)
                    setting.CloseLeg = attr.Value;

                attr = elem.Attribute("side");
                if (attr != null)
                    setting.CloseLegSide = (entity.PosiDirectionType)Enum.Parse(typeof(entity.PosiDirectionType), attr.Value);

                attr = elem.Attribute("triggerCondition");
                if (attr != null)
                {
                    setting.TriggerCondition = (CompareCondition)Enum.Parse(typeof(CompareCondition), attr.Value);
                }

                attr = elem.Attribute("threshold");
                if (attr != null)
                {
                    setting.Threshold = double.Parse(attr.Value);
                }

                return setting;
            }
            else
                throw new ArgumentException(string.Format("Unexpected strategy setting ({0})", name));
        }

        public static StrategySetting Create(string name)
        {
            if (name == ArbitrageStrategyName)
            {
                ArbitrageStrategySetting setting = new ArbitrageStrategySetting();
                setting.Direction = entity.PosiDirectionType.LONG;
                setting.OpenCondition = CompareCondition.LESS_EQUAL_THAN;
                setting.OpenThreshold = 0;
                setting.StopGainCondition = CompareCondition.GREATER_THAN;
                setting.StopGainThreshold = 0;
                setting.StopLossCondition = CompareCondition.LESS_THAN;
                setting.StopLossThreshold = 0;
                return setting;
            }
            else if(name == ChangePositionStrategyName)
            {
                ChangePositionSetting setting = new ChangePositionSetting();
                setting.TriggerCondition = CompareCondition.GREATER_EQUAL_THAN;
                setting.Threshold = 100;
                
                return setting;
            }
            else
                throw new ArgumentException(string.Format("Unexpected strategy type - {0}", name));
        }
    }
}

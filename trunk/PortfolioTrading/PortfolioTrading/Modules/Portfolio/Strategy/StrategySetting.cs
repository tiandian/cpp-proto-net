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
        public const string ScalperStrategyName = "Scalper";

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
            else if(name == ScalperStrategyName)
            {
                ScalperSetting setting = new ScalperSetting();
                XElement elem = XElement.Parse(xmlText);
                XAttribute attr = elem.Attribute("threshold");
                if (attr != null)
                {
                    setting.Threshold = double.Parse(attr.Value);
                }
                attr = elem.Attribute("prickTick");
                if (attr != null)
                {
                    setting.PriceTick = double.Parse(attr.Value);
                }
                attr = elem.Attribute("caseLE2Tick");
                if(attr != null)
                {
                    setting.CaseLE2Tick = (entity.DirectionDepends)Enum.Parse(typeof(entity.DirectionDepends), attr.Value);
                }
                attr = elem.Attribute("caseLE3Tick");
                if(attr != null)
                {
                    setting.CaseLE3Tick = (entity.DirectionDepends)Enum.Parse(typeof(entity.DirectionDepends), attr.Value);
                }
                attr = elem.Attribute("caseGE4Tick");
                if(attr != null)
                {
                    setting.CaseGE4Tick = (entity.DirectionDepends)Enum.Parse(typeof(entity.DirectionDepends), attr.Value);
                }
                attr = elem.Attribute("caseNoChange");
                if(attr != null)
                {
                    setting.CaseNoChange = (entity.DirectionDepends)Enum.Parse(typeof(entity.DirectionDepends), attr.Value);
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
                setting.StopLossCondition = CompareCondition.GREATER_THAN;
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
            else if (name == ScalperStrategyName)
            {
                ScalperSetting setting = new ScalperSetting();
                setting.Threshold = 0;
                setting.PriceTick = 0.2;
                setting.CaseLE2Tick = entity.DirectionDepends.ON_SMALL_SIZE;
                setting.CaseLE3Tick = entity.DirectionDepends.ON_BIG_SIZE;
                setting.CaseGE4Tick = entity.DirectionDepends.ON_SMALL_CHANGE;
                setting.CaseNoChange = entity.DirectionDepends.ON_BIG_SIZE;
                return setting;
            }
            else
                throw new ArgumentException(string.Format("Unexpected strategy type - {0}", name));
        }
    }
}

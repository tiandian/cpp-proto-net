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

                attr = elem.Attribute("open");
                if (attr != null)
                {
                    setting.OpenThreshold = double.Parse(attr.Value);
                }

                attr = elem.Attribute("stopGain");
                if (attr != null)
                {
                    setting.StopGainThreshold = double.Parse(attr.Value);
                }

                attr = elem.Attribute("stopLoss");
                if (attr != null)
                {
                    setting.StopLossThreshold = double.Parse(attr.Value);
                }

                return setting;
            }
            else
                throw new ArgumentException(string.Format("Unexpected strategy setting ({0})", name));
        }

        public static StrategySetting Create(string name)
        {
            ArbitrageStrategySetting setting = new ArbitrageStrategySetting();
            setting.Direction = entity.PosiDirectionType.LONG;
            setting.OpenThreshold = 100;
            setting.StopGainThreshold = 20;
            setting.StopLossThreshold = 100;
            return setting;
        }
    }
}

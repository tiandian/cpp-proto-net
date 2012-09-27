using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using APInvokeManaged;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ArbitrageStrategySetting : StrategySetting
    {
        public entity.PosiDirectionType Direction { get; set; }
        public double OpenThreshold { get; set; }
        public double StopGainThreshold { get; set; }
        public double StopLossThreshold { get; set; }

        public override string Persist()
        {
            XElement elem = new XElement("arbitrageStrategySetting");
            elem.Add(new XAttribute("direction", Direction.ToString()));
            elem.Add(new XAttribute("open", OpenThreshold));
            elem.Add(new XAttribute("stopGain", StopGainThreshold));
            elem.Add(new XAttribute("stopLoss", StopLossThreshold));
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
            entitySettings.OpenPosiThreshold = OpenThreshold;
            entitySettings.StopGainThreshold = StopGainThreshold;
            entitySettings.StopLossThreshold = StopLossThreshold;
            return DataTranslater.Serialize(entitySettings);
        }
    }
}

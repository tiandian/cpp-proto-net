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

        public override string Persist()
        {
            XElement elem = new XElement("manualSetting",
                new XAttribute("retryTimes", RetryTimes));

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
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.StrategyItem manualStrategy = new PTEntity.StrategyItem();
            manualStrategy.RetryTimes = RetryTimes;

            return manualStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ManualStrategySetting strategySetting = (ManualStrategySetting)settings;
            RetryTimes = strategySetting.RetryTimes;
        }
    }
}

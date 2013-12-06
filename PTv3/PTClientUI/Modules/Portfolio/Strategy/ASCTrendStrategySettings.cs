using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ASCTrendStrategySettings : StrategySetting
    {
        #region Symbol
        private string _symbol;

        public string Symbol
        {
            get { return _symbol; }
            set
            {
                if (_symbol != value)
                {
                    _symbol = value;
                    RaisePropertyChanged("Symbol");
                }
            }
        }
        #endregion

        #region Period
        private int _period = 60;

        public int Period
        {
            get { return _period; }
            set
            {
                if (_period != value)
                {
                    _period = value;
                    RaisePropertyChanged("Period");
                }
            }
        }
        #endregion

        #region Risk
        private int _risk;

        public int Risk
        {
            get { return _risk; }
            set
            {
                if (_risk != value)
                {
                    _risk = value;
                    RaisePropertyChanged("Risk");
                }
            }
        }
        #endregion

        #region AvergatePeriod
        private int _averagePeriod;

        public int AvergatePeriod
        {
            get { return _averagePeriod; }
            set
            {
                if (_averagePeriod != value)
                {
                    _averagePeriod = value;
                    RaisePropertyChanged("AvergatePeriod");
                }
            }
        }
        #endregion

        #region BreakoutLength
        private int _breakoutLength;

        public int BreakoutLength
        {
            get { return _breakoutLength; }
            set
            {
                if (_breakoutLength != value)
                {
                    _breakoutLength = value;
                    RaisePropertyChanged("BreakoutLength");
                }
            }
        }
        #endregion

        public override string Name
        {
            get { return ASCTrendStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("ascTrendSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("period", Period),
                new XAttribute("risk", Risk),
                new XAttribute("averagePeriod", AvergatePeriod),
                new XAttribute("breakoutLength", BreakoutLength)
                );

            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("symbol");
            if (attr != null)
            {
                Symbol = attr.Value;
            }
            attr = elem.Attribute("period");
            if (attr != null)
            {
                Period = int.Parse(attr.Value);
            }
            attr = elem.Attribute("risk");
            if (attr != null)
            {
                Risk = int.Parse(attr.Value);
            }
            attr = elem.Attribute("averagePeriod");
            if (attr != null)
            {
                AvergatePeriod = int.Parse(attr.Value);
            }

            attr = elem.Attribute("breakoutLength");
            if (attr != null)
            {
                BreakoutLength = int.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ASCTrendStrategyItem wmaStrategy = new PTEntity.ASCTrendStrategyItem();
            wmaStrategy.OpenTimeout = 500;
            wmaStrategy.RetryTimes = 8;
            wmaStrategy.Symbol = this.Symbol;
            wmaStrategy.Risk = this.Risk;
            wmaStrategy.AveragePeriod = this.AvergatePeriod;
            wmaStrategy.Period = this.Period;
            wmaStrategy.BreakoutLength = this.BreakoutLength;

            //wmaStrategy.Triggers.Add(new PTEntity.WMATrendTrailingStopTriggerItem(this.TrailingStopValue));

            return wmaStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ASCTrendStrategySettings otherSettings = (ASCTrendStrategySettings)settings;
            Risk = otherSettings.Risk;
            AvergatePeriod = otherSettings.AvergatePeriod;
            BreakoutLength = otherSettings.BreakoutLength;
            Period = otherSettings.Period;
        }
    }
}

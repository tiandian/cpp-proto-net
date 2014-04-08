using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class RangeTrendStrategySettings : StrategySetting
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
        private int _period = 300;

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

        #region OpenPeriod
        private int _openPeriod;

        public int OpenPeriod
        {
            get { return _openPeriod; }
            set
            {
                if (_openPeriod != value)
                {
                    _openPeriod = value;
                    RaisePropertyChanged("OpenPeriod");
                }
            }
        }
        #endregion

        #region ClosePeriod
        private int _closePeriod;

        public int ClosePeriod
        {
            get { return _closePeriod; }
            set
            {
                if (_closePeriod != value)
                {
                    _closePeriod = value;
                    RaisePropertyChanged("ClosePeriod");
                }
            }
        }
        #endregion

        #region StopLossFactor
        private double _stopLossFactor;

        public double StopLossFactor
        {
            get { return _stopLossFactor; }
            set
            {
                if (_stopLossFactor != value)
                {
                    _stopLossFactor = value;
                    RaisePropertyChanged("StopLossFactor");
                }
            }
        }
        #endregion

        #region TrendFactor
        private double _trendFactor;

        public double TrendFactor
        {
            get { return _trendFactor; }
            set
            {
                if (_trendFactor != value)
                {
                    _trendFactor = value;
                    RaisePropertyChanged("TrendFactor");
                }
            }
        }
        #endregion


        public override string Name
        {
            get { return RangeTrendStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("rangeTrendSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("period", Period),
                new XAttribute("openPeriod", OpenPeriod),
                new XAttribute("closePeriod", ClosePeriod),
                new XAttribute("stopLossFactor", StopLossFactor),
                new XAttribute("trendFactor", TrendFactor)
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
            attr = elem.Attribute("openPeriod");
            if (attr != null)
            {
                OpenPeriod = int.Parse(attr.Value);
            }
            attr = elem.Attribute("closePeriod");
            if (attr != null)
            {
                ClosePeriod = int.Parse(attr.Value);
            }

            attr = elem.Attribute("stopLossFactor");
            if (attr != null)
            {
                StopLossFactor = double.Parse(attr.Value);
            }

            attr = elem.Attribute("trendFactor");
            if (attr != null)
            {
                TrendFactor = double.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.RangeTrendStrategyItem rtStrategy = new PTEntity.RangeTrendStrategyItem();
            rtStrategy.OpenTimeout = 500;
            rtStrategy.RetryTimes = 8;
            rtStrategy.Symbol = this.Symbol;
            rtStrategy.Period = this.Period;
            rtStrategy.OpenPeriod = this.OpenPeriod;
            rtStrategy.ClosePeriod = this.ClosePeriod;
            rtStrategy.StopLossFactor = this.StopLossFactor;
            rtStrategy.TrendFactor = this.TrendFactor;

            //wmaStrategy.Triggers.Add(new PTEntity.WMATrendTrailingStopTriggerItem(this.TrailingStopValue));

            return rtStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            RangeTrendStrategySettings otherSettings = (RangeTrendStrategySettings)settings;
            OpenPeriod = otherSettings.OpenPeriod;
            ClosePeriod = otherSettings.ClosePeriod;
            StopLossFactor = otherSettings.StopLossFactor;
            TrendFactor = otherSettings.TrendFactor;

            Period = otherSettings.Period;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class MACDHistSlopeStrategySetting : StrategySetting
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

        #region MACD_Short
        private int _macd_short = 12;

        public int MACD_Short
        {
            get { return _macd_short; }
            set
            {
                if (_macd_short != value)
                {
                    _macd_short = value;
                    RaisePropertyChanged("MACD_Short");
                }
            }
        }
        #endregion

        #region MACD_Long
        private int _macd_long = 26;

        public int MACD_Long
        {
            get { return _macd_long; }
            set
            {
                if (_macd_long != value)
                {
                    _macd_long = value;
                    RaisePropertyChanged("MACD_Long");
                }
            }
        }
        #endregion

        #region MACD_M
        private int _macd_m = 9;

        public int MACD_M
        {
            get { return _macd_m; }
            set
            {
                if (_macd_m != value)
                {
                    _macd_m = value;
                    RaisePropertyChanged("MACD_M");
                }
            }
        }
        #endregion

        #region FastPeriod
        private int _fastPeriod = 60;

        public int FastPeriod
        {
            get { return _fastPeriod; }
            set
            {
                if (_fastPeriod != value)
                {
                    _fastPeriod = value;
                    RaisePropertyChanged("FastPeriod");
                }
            }
        }
        #endregion

        #region SlowPeriod
        private int _slowPeriod = 300;

        public int SlowPeriod
        {
            get { return _slowPeriod; }
            set
            {
                if (_slowPeriod != value)
                {
                    _slowPeriod = value;
                    RaisePropertyChanged("SlowPeriod");
                }
            }
        }
        #endregion

        public override string Name
        {
            get { return MACDHistSlopeStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("macdHistSlopeSetting",
                new XAttribute("short", MACD_Short),
                new XAttribute("long", MACD_Long),
                new XAttribute("m", MACD_M),
                new XAttribute("fastPeriod", FastPeriod),
                new XAttribute("slowPeriod", SlowPeriod)
                );

            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("short");
            if (attr != null)
            {
                MACD_Short = int.Parse(attr.Value);
            }
            attr = elem.Attribute("long");
            if (attr != null)
            {
                MACD_Long = int.Parse(attr.Value);
            }
            attr = elem.Attribute("m");
            if (attr != null)
            {
                MACD_M = int.Parse(attr.Value);
            }
            attr = elem.Attribute("fastPeriod");
            if (attr != null)
            {
                FastPeriod = int.Parse(attr.Value);
            }
            attr = elem.Attribute("slowPeriod");
            if (attr != null)
            {
                SlowPeriod = int.Parse(attr.Value);
            }
            
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            throw new NotImplementedException();
        }

        public override void CopyFrom(StrategySetting settings)
        {
            throw new NotImplementedException();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class MACDCrossStrategySettings : StrategySetting
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

        #region FastShortSeed
        private double _fastShortSeed;

        public double FastShortSeed
        {
            get { return _fastShortSeed; }
            set
            {
                if (_fastShortSeed != value)
                {
                    _fastShortSeed = value;
                    RaisePropertyChanged("FastShortSeed");
                }
            }
        }
        #endregion

        #region FastLongSeed
        private double _fastLongSeed;

        public double FastLongSeed
        {
            get { return _fastLongSeed; }
            set
            {
                if (_fastLongSeed != value)
                {
                    _fastLongSeed = value;
                    RaisePropertyChanged("FastLongSeed");
                }
            }
        }
        #endregion

        #region FastSignalSeed
        private double _fastSignalSeed;

        public double FastSignalSeed
        {
            get { return _fastSignalSeed; }
            set
            {
                if (_fastSignalSeed != value)
                {
                    _fastSignalSeed = value;
                    RaisePropertyChanged("FastSignalSeed");
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

        #region SlowShortSeed
        private double _slowShortSeed;

        public double SlowShortSeed
        {
            get { return _slowShortSeed; }
            set
            {
                if (_slowShortSeed != value)
                {
                    _slowShortSeed = value;
                    RaisePropertyChanged("SlowShortSeed");
                }
            }
        }
        #endregion

        #region SlowLongSeed
        private double _slowLongSeed;

        public double SlowLongSeed
        {
            get { return _slowLongSeed; }
            set
            {
                if (_slowLongSeed != value)
                {
                    _slowLongSeed = value;
                    RaisePropertyChanged("SlowLongSeed");
                }
            }
        }
        #endregion

        #region SlowSignalSeed
        private double _slowSignalSeed;

        public double SlowSignalSeed
        {
            get { return _slowSignalSeed; }
            set
            {
                if (_slowSignalSeed != value)
                {
                    _slowSignalSeed = value;
                    RaisePropertyChanged("SlowSignalSeed");
                }
            }
        }
        #endregion

        #region Boll_M
        private int _boll_M;

        public int Boll_M
        {
            get { return _boll_M; }
            set
            {
                if (_boll_M != value)
                {
                    _boll_M = value;
                    RaisePropertyChanged("Boll_M");
                }
            }
        }
        #endregion

        #region Boll_P
        private int _boll_p;

        public int Boll_P
        {
            get { return _boll_p; }
            set
            {
                if (_boll_p != value)
                {
                    _boll_p = value;
                    RaisePropertyChanged("Boll_P");
                }
            }
        }
        #endregion
        
        #region TrailingStopValue
        private double _trailingStopValue;

        public double TrailingStopValue
        {
            get { return _trailingStopValue; }
            set
            {
                if (_trailingStopValue != value)
                {
                    _trailingStopValue = value;
                    RaisePropertyChanged("TrailingStopValue");
                }
            }
        }
        #endregion

        public override string Name
        {
            get { return MACDCrossStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("macdCrossSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("short", MACD_Short),
                new XAttribute("long", MACD_Long),
                new XAttribute("m", MACD_M),
                new XAttribute("fastPeriod", FastPeriod),
                new XAttribute("fastShortSeed", FastShortSeed),
                new XAttribute("fastLongSeed", FastLongSeed),
                new XAttribute("fastSignalSeed", FastSignalSeed),
                new XAttribute("slowPeriod", SlowPeriod),
                new XAttribute("slowShortSeed", SlowShortSeed),
                new XAttribute("slowLongSeed", SlowLongSeed),
                new XAttribute("slowSignalSeed", SlowSignalSeed),
                new XAttribute("bollM", Boll_M),
                new XAttribute("bollP", Boll_P),
                new XAttribute("trailingStopValue", TrailingStopValue)
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
            attr = elem.Attribute("short");
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
            attr = elem.Attribute("fastShortSeed");
            if (attr != null)
            {
                FastShortSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("fastLongSeed");
            if (attr != null)
            {
                FastLongSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("fastSignalSeed");
            if (attr != null)
            {
                FastSignalSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("slowPeriod");
            if (attr != null)
            {
                SlowPeriod = int.Parse(attr.Value);
            }
            attr = elem.Attribute("slowShortSeed");
            if (attr != null)
            {
                SlowShortSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("slowLongSeed");
            if (attr != null)
            {
                SlowLongSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("slowSignalSeed");
            if (attr != null)
            {
                SlowSignalSeed = double.Parse(attr.Value);
            }
            attr = elem.Attribute("bollM");
            if (attr != null)
            {
                Boll_M = int.Parse(attr.Value);
            }
            attr = elem.Attribute("bollP");
            if (attr != null)
            {
                Boll_P = int.Parse(attr.Value);
            }
            attr = elem.Attribute("trailingStopValue");
            if (attr != null)
            {
                TrailingStopValue = double.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.MACDCrossStrategyItem crossStrategy = new PTEntity.MACDCrossStrategyItem();
            crossStrategy.OpenTimeout = 500;
            crossStrategy.RetryTimes = 8;
            crossStrategy.Symbol = this.Symbol;
            crossStrategy.Short = this.MACD_Short;
            crossStrategy.Long = this.MACD_Long;
            crossStrategy.M = this.MACD_M;
            crossStrategy.FastPeriod = this.FastPeriod;
            crossStrategy.SlowPeriod = this.SlowPeriod;

            crossStrategy.BollM = this.Boll_M;
            crossStrategy.BollP = this.Boll_P;

            crossStrategy.FastShortSeed = this.FastShortSeed;
            crossStrategy.FastLongSeed = this.FastLongSeed;
            crossStrategy.FastSignalSeed = this.FastSignalSeed;

            crossStrategy.SlowShortSeed = this.SlowShortSeed;
            crossStrategy.SlowLongSeed = this.SlowLongSeed;
            crossStrategy.SlowSignalSeed = this.SlowSignalSeed;

            crossStrategy.Triggers.Add(new PTEntity.CrossOpenTriggerItem() { Enabled = true });
            crossStrategy.Triggers.Add(new PTEntity.CrossCloseTriggerItem() { Enabled = true });
            crossStrategy.Triggers.Add(new PTEntity.CrossTrailingStopTriggerItem(this.TrailingStopValue));

            return crossStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            MACDCrossStrategySettings otherSettings = (MACDCrossStrategySettings)settings;
            Symbol = otherSettings.Symbol;
            MACD_Short = otherSettings.MACD_Short;
            MACD_Long = otherSettings.MACD_Long;
            MACD_M = otherSettings.MACD_M;
            Boll_M = otherSettings.Boll_M;
            Boll_P = otherSettings.Boll_P;
            FastPeriod = otherSettings.FastPeriod;
            FastShortSeed = otherSettings.FastShortSeed;
            FastLongSeed = otherSettings.FastLongSeed;
            FastSignalSeed = otherSettings.FastSignalSeed;
            SlowPeriod = otherSettings.SlowPeriod;
            SlowShortSeed = otherSettings.SlowShortSeed;
            SlowLongSeed = otherSettings.SlowLongSeed;
            SlowSignalSeed = otherSettings.SlowSignalSeed;
            TrailingStopValue = otherSettings.TrailingStopValue;
        }
    }
}

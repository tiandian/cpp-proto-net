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

        #region FastStdDiff
        private double _fastStdDiff;

        public double FastStdDiff
        {
            get { return _fastStdDiff; }
            set
            {
                if (_fastStdDiff != value)
                {
                    _fastStdDiff = value;
                    RaisePropertyChanged("FastStdDiff");
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

        #region SlowStdDiff
        private double _slowStdDiff;

        public double SlowStdDiff
        {
            get { return _slowStdDiff; }
            set
            {
                if (_slowStdDiff != value)
                {
                    _slowStdDiff = value;
                    RaisePropertyChanged("SlowStdDiff");
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


        #region FastAngleThreshold
        private int _fastAngleThreshold;

        public int FastAngleThreshold
        {
            get { return _fastAngleThreshold; }
            set
            {
                if (_fastAngleThreshold != value)
                {
                    _fastAngleThreshold = value;
                    RaisePropertyChanged("FastAngleThreshold");
                }
            }
        }
        #endregion

        #region SlowAngleThreshold
        private int _slowAngleThreshold;

        public int SlowAngleThreshold
        {
            get { return _slowAngleThreshold; }
            set
            {
                if (_slowAngleThreshold != value)
                {
                    _slowAngleThreshold = value;
                    RaisePropertyChanged("SlowAngleThreshold");
                }
            }
        }
        #endregion

        #region FastReverseAngle
        private int _fastReverseAngle;

        public int FastReverseAngle
        {
            get { return _fastReverseAngle; }
            set
            {
                if (_fastReverseAngle != value)
                {
                    _fastReverseAngle = value;
                    RaisePropertyChanged("FastReverseAngle");
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
            get { return MACDHistSlopeStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("macdHistSlopeSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("short", MACD_Short),
                new XAttribute("long", MACD_Long),
                new XAttribute("m", MACD_M),
                new XAttribute("fastPeriod", FastPeriod),
                new XAttribute("fastStdDiff", FastStdDiff),
                new XAttribute("fastShortSeed", FastShortSeed),
                new XAttribute("fastLongSeed", FastLongSeed),
                new XAttribute("fastSignalSeed", FastSignalSeed),
                new XAttribute("fastAngleThreshold", FastAngleThreshold),
                new XAttribute("slowPeriod", SlowPeriod),
                new XAttribute("slowStdDiff", SlowStdDiff),
                new XAttribute("slowShortSeed", SlowShortSeed),
                new XAttribute("slowLongSeed", SlowLongSeed),
                new XAttribute("slowSignalSeed", SlowSignalSeed),
                new XAttribute("slowAngleThreshold", SlowAngleThreshold),
                new XAttribute("fastReverseAngle", FastReverseAngle),
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
            attr = elem.Attribute("fastStdDiff");
            if (attr != null)
            {
                FastStdDiff = double.Parse(attr.Value);
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
            attr = elem.Attribute("fastAngleThreshold");
            if (attr != null)
            {
                FastAngleThreshold = int.Parse(attr.Value);
            }
            attr = elem.Attribute("slowPeriod");
            if (attr != null)
            {
                SlowPeriod = int.Parse(attr.Value);
            }
            attr = elem.Attribute("slowStdDiff");
            if (attr != null)
            {
                SlowStdDiff = double.Parse(attr.Value);
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
            attr = elem.Attribute("slowAngleThreshold");
            if (attr != null)
            {
                SlowAngleThreshold = int.Parse(attr.Value);
            }
            attr = elem.Attribute("fastReverseAngle");
            if (attr != null)
            {
                FastReverseAngle = int.Parse(attr.Value);
            }
            attr = elem.Attribute("trailingStopValue");
            if (attr != null)
            {
                TrailingStopValue = double.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.MACDSlopeStrategyItem macdSlopeStrategy = new PTEntity.MACDSlopeStrategyItem();
            macdSlopeStrategy.OpenTimeout = 500;
            macdSlopeStrategy.RetryTimes = 8;
            macdSlopeStrategy.Symbol = this.Symbol;
            macdSlopeStrategy.Short = this.MACD_Short;
            macdSlopeStrategy.Long = this.MACD_Long;
            macdSlopeStrategy.M = this.MACD_M;
            macdSlopeStrategy.FastPeriod = this.FastPeriod;
            macdSlopeStrategy.FastStdDiff = this.FastStdDiff;
            macdSlopeStrategy.SlowPeriod = this.SlowPeriod;
            macdSlopeStrategy.SlowStdDiff = this.SlowStdDiff;

            macdSlopeStrategy.FastShortSeed = this.FastShortSeed;
            macdSlopeStrategy.FastLongSeed = this.FastLongSeed;
            macdSlopeStrategy.FastSignalSeed = this.FastSignalSeed;

            macdSlopeStrategy.SlowShortSeed = this.SlowShortSeed;
            macdSlopeStrategy.SlowLongSeed = this.SlowLongSeed;
            macdSlopeStrategy.SlowSignalSeed = this.SlowSignalSeed;
            
            macdSlopeStrategy.Triggers.Add(new PTEntity.HistSlopeTriggerItem
            {
                Offset = PTEntity.PosiOffsetFlag.OPEN,
                FastAngleThreshold = this.FastAngleThreshold,
                SlowAngleThreshold = this.SlowAngleThreshold,
                Enabled = true
            });
            macdSlopeStrategy.Triggers.Add(new PTEntity.HistSlopeTriggerItem
            {
                Offset = PTEntity.PosiOffsetFlag.CLOSE,
                FastAngleThreshold = this.FastReverseAngle,
                SlowAngleThreshold = this.SlowAngleThreshold,
                Enabled = true
            });
            macdSlopeStrategy.Triggers.Add(new PTEntity.HistSlopeTrailingStopTriggerItem(this.TrailingStopValue));

            return macdSlopeStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            MACDHistSlopeStrategySetting otherSettings = (MACDHistSlopeStrategySetting)settings;
            Symbol = otherSettings.Symbol;
            MACD_Short = otherSettings.MACD_Short;
            MACD_Long = otherSettings.MACD_Long;
            MACD_M = otherSettings.MACD_M;
            FastPeriod = otherSettings.FastPeriod;
            FastStdDiff = otherSettings.FastStdDiff;
            FastShortSeed = otherSettings.FastShortSeed;
            FastLongSeed = otherSettings.FastLongSeed;
            FastSignalSeed = otherSettings.FastSignalSeed;
            FastAngleThreshold = otherSettings.FastAngleThreshold;
            SlowPeriod = otherSettings.SlowPeriod;
            SlowStdDiff = otherSettings.SlowStdDiff;
            SlowShortSeed = otherSettings.SlowShortSeed;
            SlowLongSeed = otherSettings.SlowLongSeed;
            SlowSignalSeed = otherSettings.SlowSignalSeed;
            SlowAngleThreshold = otherSettings.SlowAngleThreshold;
            FastReverseAngle = otherSettings.FastReverseAngle;
            TrailingStopValue = otherSettings.TrailingStopValue;
        }
    }
}

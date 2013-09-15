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
                new XAttribute("fastAngleThreshold", FastAngleThreshold),
                new XAttribute("slowPeriod", SlowPeriod),
                new XAttribute("slowStdDiff", SlowStdDiff),
                new XAttribute("slowAngleThreshold", SlowAngleThreshold),
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
            attr = elem.Attribute("slowAngleThreshold");
            if (attr != null)
            {
                SlowAngleThreshold = int.Parse(attr.Value);
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
            macdSlopeStrategy.Symbol = this.Symbol;
            macdSlopeStrategy.Short = this.MACD_Short;
            macdSlopeStrategy.Long = this.MACD_Long;
            macdSlopeStrategy.M = this.MACD_M;
            macdSlopeStrategy.FastPeriod = this.FastPeriod;
            macdSlopeStrategy.FastStdDiff = this.FastStdDiff;
            macdSlopeStrategy.SlowPeriod = this.SlowPeriod;
            macdSlopeStrategy.SlowStdDiff = this.SlowStdDiff;
            
            macdSlopeStrategy.Triggers.Add(new PTEntity.HistSlopeTriggerItem
            {
                Offset = PTEntity.PosiOffsetFlag.OPEN,
                FastAngleThreshold = this.FastAngleThreshold,
                SlowAngleThreshold = this.SlowAngleThreshold
            });
            macdSlopeStrategy.Triggers.Add(new PTEntity.HistSlopeTriggerItem
            {
                Offset = PTEntity.PosiOffsetFlag.CLOSE,
                FastAngleThreshold = this.FastAngleThreshold,
                SlowAngleThreshold = this.SlowAngleThreshold
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
            FastAngleThreshold = otherSettings.FastAngleThreshold;
            SlowPeriod = otherSettings.SlowPeriod;
            SlowStdDiff = otherSettings.SlowStdDiff;
            SlowAngleThreshold = otherSettings.SlowAngleThreshold;
            TrailingStopValue = otherSettings.TrailingStopValue;
        }
    }
}

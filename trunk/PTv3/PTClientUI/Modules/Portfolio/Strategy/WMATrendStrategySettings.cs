using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class WMATrendStrategySettings : StrategySetting
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

        #region WMA_Param
        private int _wma_param;

        public int WMA_Param
        {
            get { return _wma_param; }
            set
            {
                if (_wma_param != value)
                {
                    _wma_param = value;
                    RaisePropertyChanged("WMA_Param");
                }
            }
        }
        #endregion

        #region MA_N
        private int _ma_n;

        public int MA_N
        {
            get { return _ma_n; }
            set
            {
                if (_ma_n != value)
                {
                    _ma_n = value;
                    RaisePropertyChanged("MA_N");
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
            get { return WMATrendStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("wmaTrendSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("wmaParam", WMA_Param),
                new XAttribute("maN", MA_N),
                new XAttribute("period", Period),
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
            attr = elem.Attribute("wmaParam");
            if (attr != null)
            {
                WMA_Param = int.Parse(attr.Value);
            }
            attr = elem.Attribute("maN");
            if (attr != null)
            {
                MA_N = int.Parse(attr.Value);
            }
            attr = elem.Attribute("period");
            if (attr != null)
            {
                Period = int.Parse(attr.Value);
            }
            attr = elem.Attribute("trailingStopValue");
            if (attr != null)
            {
                TrailingStopValue = double.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.WMATrendStrategyItem wmaStrategy = new PTEntity.WMATrendStrategyItem();
            wmaStrategy.OpenTimeout = 500;
            wmaStrategy.RetryTimes = 8;
            wmaStrategy.Symbol = this.Symbol;
            wmaStrategy.WMA_Param = this.WMA_Param;
            wmaStrategy.MA_N = this.MA_N;
            wmaStrategy.Period = this.Period;
            wmaStrategy.TrailingStopValue = this.TrailingStopValue;
            
            wmaStrategy.Triggers.Add(new PTEntity.WMATrendTrailingStopTriggerItem(this.TrailingStopValue));

            return wmaStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            WMATrendStrategySettings otherSettings = (WMATrendStrategySettings)settings;
            Symbol = otherSettings.Symbol;
            WMA_Param = otherSettings.WMA_Param;
            MA_N = otherSettings.MA_N;
            Period = otherSettings.Period;
            TrailingStopValue = otherSettings.TrailingStopValue;
        }
    }
}

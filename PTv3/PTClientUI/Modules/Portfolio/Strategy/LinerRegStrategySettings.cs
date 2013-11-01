using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class LinerRegStrategySettings : StrategySetting
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

        #region Number
        private int _number = 3;

        public int Number
        {
            get { return _number; }
            set
            {
                if (_number != value)
                {
                    _number = value;
                    RaisePropertyChanged("Number");
                }
            }
        }
        #endregion

        #region OpenThreshold
        private double _openThreshold;

        public double OpenThreshold
        {
            get { return _openThreshold; }
            set
            {
                if (_openThreshold != value)
                {
                    _openThreshold = value;
                    RaisePropertyChanged("OpenThreshold");
                }
            }
        }
        #endregion

        #region CloseThreshold
        private double _closeThreshold;

        public double CloseThreshold
        {
            get { return _closeThreshold; }
            set
            {
                if (_closeThreshold != value)
                {
                    _closeThreshold = value;
                    RaisePropertyChanged("CloseThreshold");
                }
            }
        }
        #endregion

        public override string Name
        {
            get { return LinerRegressionStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("linerRegressionSetting",
                new XAttribute("symbol", Symbol),
                new XAttribute("period", Period),
                new XAttribute("number", Number),
                new XAttribute("openThreshold", OpenThreshold),
                new XAttribute("closeThreshold", CloseThreshold)
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
            attr = elem.Attribute("number");
            if (attr != null)
            {
                Number = int.Parse(attr.Value);
            }
            attr = elem.Attribute("openThreshold");
            if (attr != null)
            {
                OpenThreshold = double.Parse(attr.Value);
            }
            attr = elem.Attribute("closeThreshold");
            if (attr != null)
            {
                CloseThreshold = double.Parse(attr.Value);
            }
        }

        public override void CopyFrom(StrategySetting settings)
        {
            LinerRegStrategySettings otherSettings = (LinerRegStrategySettings)settings;
            Symbol = otherSettings.Symbol;
            Period = otherSettings.Period;
            Number = otherSettings.Number;
            OpenThreshold = otherSettings.OpenThreshold;
            CloseThreshold = otherSettings.CloseThreshold;
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.LinerRegressionStrategyItem linerRegStrategy = new PTEntity.LinerRegressionStrategyItem();
            
            linerRegStrategy.Period = this.Period;
            linerRegStrategy.Number = this.Number;
            linerRegStrategy.OpenThreshold = this.OpenThreshold;
            linerRegStrategy.CloseThreshold = this.CloseThreshold;
            linerRegStrategy.Symbol = this.Symbol;

            return linerRegStrategy;
        }
    }
}

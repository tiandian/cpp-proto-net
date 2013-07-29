using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ScalperSetting : StrategySetting
    {
        #region Threshold
        private double _diff;

        public double Threshold
        {
            get { return _diff; }
            set
            {
                if (_diff != value)
                {
                    _diff = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion

        #region PriceTick
        private double _priceTick;

        public double PriceTick
        {
            get { return _priceTick; }
            set
            {
                if (_priceTick != value)
                {
                    _priceTick = value;
                    RaisePropertyChanged("PriceTick");
                }
            }
        }
        #endregion

        #region CaseLE2Tick
        private PTEntity.DirectionDepends _caseLE2Tick;

        public PTEntity.DirectionDepends CaseLE2Tick
        {
            get { return _caseLE2Tick; }
            set
            {
                if (_caseLE2Tick != value)
                {
                    _caseLE2Tick = value;
                    RaisePropertyChanged("CaseLE2Tick");
                }
            }
        }
        #endregion

        #region CaseLE3Tick
        private PTEntity.DirectionDepends _caseLE3Tick;

        public PTEntity.DirectionDepends CaseLE3Tick
        {
            get { return _caseLE3Tick; }
            set
            {
                if (_caseLE3Tick != value)
                {
                    _caseLE3Tick = value;
                    RaisePropertyChanged("CaseLE3Tick");
                }
            }
        }
        #endregion

        #region CaseGE4Tick
        private PTEntity.DirectionDepends _caseGE4Tick;

        public PTEntity.DirectionDepends CaseGE4Tick
        {
            get { return _caseGE4Tick; }
            set
            {
                if (_caseGE4Tick != value)
                {
                    _caseGE4Tick = value;
                    RaisePropertyChanged("CaseGE4Tick");
                }
            }
        }
        #endregion

        #region CaseNoChange
        private PTEntity.DirectionDepends _caseNoChange;

        public PTEntity.DirectionDepends CaseNoChange
        {
            get { return _caseNoChange; }
            set
            {
                if (_caseNoChange != value)
                {
                    _caseNoChange = value;
                    RaisePropertyChanged("CaseNoChange");
                }
            }
        }
        #endregion

        #region StopLossCloseMethod
        private PTEntity.StopLossCloseMethods _closeMethod;

        public PTEntity.StopLossCloseMethods StopLossCloseMethod
        {
            get { return _closeMethod; }
            set
            {
                if (_closeMethod != value)
                {
                    _closeMethod = value;
                    RaisePropertyChanged("StopLossCloseMethod");
                }
            }
        }
        #endregion

        #region RetryTimes
        private int _retryTimes;

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

        #region OpenTimeout
        private int _openTimeout;

        public int OpenTimeout
        {
            get { return _openTimeout; }
            set
            {
                if (_openTimeout != value)
                {
                    _openTimeout = value;
                    RaisePropertyChanged("OpenTimeout");
                }
            }
        }
        #endregion

        public ScalperSetting()
        {
            Threshold = 0;
            PriceTick = 0.2;
            CaseLE2Tick = PTEntity.DirectionDepends.ON_SMALL_SIZE;
            CaseLE3Tick = PTEntity.DirectionDepends.ON_BIG_SIZE;
            CaseGE4Tick = PTEntity.DirectionDepends.ON_SMALL_CHANGE;
            CaseNoChange = PTEntity.DirectionDepends.ON_BIG_SIZE;
            StopLossCloseMethod = PTEntity.StopLossCloseMethods.BASED_ON_NEXT_QUOTE;
            RetryTimes = 8;
            OpenTimeout = 100;
        }


        public override string Name
        {
            get { return ScalperStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("scalperStrategySetting",
                new XAttribute("threshold", Threshold),
                new XAttribute("prickTick", PriceTick),
                new XAttribute("caseLE2Tick", CaseLE2Tick),
                new XAttribute("caseLE3Tick", CaseLE3Tick),
                new XAttribute("caseGE4Tick", CaseGE4Tick),
                new XAttribute("caseNoChange", CaseNoChange),
                new XAttribute("stopLossCloseStrategy", StopLossCloseMethod),
                new XAttribute("retryTimes", RetryTimes),
                new XAttribute("openTimeout", OpenTimeout));

            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("threshold");
            if (attr != null)
            {
                Threshold = double.Parse(attr.Value);
            }
            attr = elem.Attribute("prickTick");
            if (attr != null)
            {
                PriceTick = double.Parse(attr.Value);
            }
            attr = elem.Attribute("caseLE2Tick");
            if (attr != null)
            {
                CaseLE2Tick = (PTEntity.DirectionDepends)Enum.Parse(typeof(PTEntity.DirectionDepends), attr.Value);
            }
            attr = elem.Attribute("caseLE3Tick");
            if (attr != null)
            {
                CaseLE3Tick = (PTEntity.DirectionDepends)Enum.Parse(typeof(PTEntity.DirectionDepends), attr.Value);
            }
            attr = elem.Attribute("caseGE4Tick");
            if (attr != null)
            {
                CaseGE4Tick = (PTEntity.DirectionDepends)Enum.Parse(typeof(PTEntity.DirectionDepends), attr.Value);
            }
            attr = elem.Attribute("caseNoChange");
            if (attr != null)
            {
                CaseNoChange = (PTEntity.DirectionDepends)Enum.Parse(typeof(PTEntity.DirectionDepends), attr.Value);
            }
            attr = elem.Attribute("stopLossCloseStrategy");
            if (attr != null)
            {
                StopLossCloseMethod = (PTEntity.StopLossCloseMethods)Enum.Parse(typeof(PTEntity.StopLossCloseMethods), attr.Value);
            }
            attr = elem.Attribute("retryTimes");
            if (attr != null)
            {
                RetryTimes = int.Parse(attr.Value);
            }
            attr = elem.Attribute("openTimeout");
            if (attr != null)
            {
                OpenTimeout = int.Parse(attr.Value);
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ScalperStrategyItem scalperStrategy = new PTEntity.ScalperStrategyItem();
            scalperStrategy.PriceTick = PriceTick;
            scalperStrategy.CaseLE2Tick = CaseLE2Tick;
            scalperStrategy.CaseLE3Tick = CaseLE3Tick;
            scalperStrategy.CaseGE4Tick = CaseGE4Tick;
            scalperStrategy.CaseNoChange = CaseNoChange;
            scalperStrategy.OpenTimeout = OpenTimeout;
            scalperStrategy.RetryTimes = RetryTimes;
            scalperStrategy.StopLossStrategy = StopLossCloseMethod;

            scalperStrategy.Triggers.Add(new PTEntity.ScalperTriggerItem(Threshold) {  Enabled = true });

            return scalperStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ScalperSetting strategySettings = (ScalperSetting)settings;
            Threshold = strategySettings.Threshold;
            PriceTick = strategySettings.PriceTick;
            CaseLE2Tick = strategySettings.CaseLE2Tick;
            CaseLE3Tick = strategySettings.CaseLE3Tick;
            CaseGE4Tick = strategySettings.CaseGE4Tick;
            CaseNoChange = strategySettings.CaseNoChange;
            StopLossCloseMethod = strategySettings.StopLossCloseMethod;
            RetryTimes = strategySettings.RetryTimes;
            OpenTimeout = strategySettings.OpenTimeout;
        }
    }
}

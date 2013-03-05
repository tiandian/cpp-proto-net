using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Xml.Linq;
using APInvokeManaged;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ScalperSetting : StrategySetting, INotifyPropertyChanged
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
        private entity.DirectionDepends _caseLE2Tick;

        public entity.DirectionDepends CaseLE2Tick
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
        private entity.DirectionDepends _caseLE3Tick;

        public entity.DirectionDepends CaseLE3Tick
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
        private entity.DirectionDepends _caseGE4Tick;

        public entity.DirectionDepends CaseGE4Tick
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
        private entity.DirectionDepends _caseNoChange;

        public entity.DirectionDepends CaseNoChange
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
        private entity.StopLossCloseMethods _closeMethod;

        public entity.StopLossCloseMethods StopLossCloseMethod
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
                new XAttribute("stopLossCloseStrategy", StopLossCloseMethod));

            return elem.ToString();
        }

        public override byte[] Serialize()
        {
            entity.ScalperSettings setting = new entity.ScalperSettings();
            setting.Threshold = Threshold;
            setting.PriceTick = PriceTick;
            setting.CaseGE4Tick = CaseGE4Tick;
            setting.CaseLE2Tick = CaseLE2Tick;
            setting.CaseLE3Tick = CaseLE3Tick;
            setting.CaseNoChange = CaseNoChange;
            setting.StopLossStrategy = StopLossCloseMethod;
            return DataTranslater.Serialize(setting);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string propName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }
    }
}

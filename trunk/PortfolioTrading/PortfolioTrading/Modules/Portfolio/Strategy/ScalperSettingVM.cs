using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ScalperSettingVM : StrategySettingVM
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
        private double _priceTick = 0;

        public double PriceTick
        {
            get { return _priceTick; }
            set
            {
                if (_priceTick != value)
                {
                    _priceTick = value;
                    RaisePropertyChanged("PriceTick");
                    RaisePropertyChanged("TwoTick");
                    RaisePropertyChanged("ThreeTick");
                    RaisePropertyChanged("FourTick");
                }
            }
        }
        #endregion

        #region TwoTick

        public double TwoTick
        {
            get { return _priceTick * 2; }
        }
        #endregion

        #region ThreeTick

        public double ThreeTick
        {
            get { return _priceTick * 3; }
        }
        #endregion

        #region FourTick

        public double FourTick
        {
            get { return _priceTick * 4; }
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

        protected override void OnApplySetting()
        {
            ScalperSetting settings = (ScalperSetting)CurrentPortfolio.StrategySetting;

            settings.Threshold = this.Threshold;
            settings.PriceTick = this.PriceTick;
            settings.CaseGE4Tick = this.CaseGE4Tick;
            settings.CaseLE2Tick = this.CaseLE2Tick;
            settings.CaseLE3Tick = this.CaseLE3Tick;
            settings.CaseNoChange = this.CaseNoChange;

            base.OnApplySetting();
        }

        protected override void OnSetPortfolio(Account.PortfolioVM portfVm)
        {
            ScalperSetting settings = (ScalperSetting)portfVm.StrategySetting;
            this.Threshold = settings.Threshold;
            this.PriceTick = settings.PriceTick;
            this.CaseGE4Tick = settings.CaseGE4Tick;
            this.CaseLE2Tick = settings.CaseLE2Tick;
            this.CaseLE3Tick = settings.CaseLE3Tick;
            this.CaseNoChange = settings.CaseNoChange;
        }

        public List<DirectionDependsItem> _directionDependsItems = new List<DirectionDependsItem>();
        public IEnumerable<DirectionDependsItem> DependsItems
        {
            get { return _directionDependsItems; }
        }

        public ScalperSettingVM()
        {
            _directionDependsItems.Add(new DirectionDependsItem
            {
                Value = entity.DirectionDepends.ON_SMALL_SIZE,
                DisplayName = "挂单少的方向开仓"
            });
            _directionDependsItems.Add(new DirectionDependsItem
            {
                Value = entity.DirectionDepends.ON_BIG_SIZE,
                DisplayName = "挂单多的方向开仓"
            });
            _directionDependsItems.Add(new DirectionDependsItem
            {
                Value = entity.DirectionDepends.ON_SMALL_CHANGE,
                DisplayName = "变动小的方向开仓"
            });
            _directionDependsItems.Add(new DirectionDependsItem
            {
                Value = entity.DirectionDepends.IGNORE_THIS,
                DisplayName = "忽略"
            });
        }
    }

    public class DirectionDependsItem
    {
        public entity.DirectionDepends Value { get; set; }
        public string DisplayName { get; set; }
    }
}

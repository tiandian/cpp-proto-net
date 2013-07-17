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
        #region TwoTick

        public double TwoTick
        {
            get { return _settings.PriceTick * 2; }
        }
        #endregion

        #region ThreeTick

        public double ThreeTick
        {
            get { return _settings.PriceTick * 3; }
        }
        #endregion

        #region FourTick

        public double FourTick
        {
            get { return _settings.PriceTick * 4; }
        }
        #endregion

        private ScalperSetting _settings;

        protected override StrategySetting CreateSettings()
        {
            _settings = new ScalperSetting();
            _settings.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(settings_PropertyChanged);
            return _settings;
        }

        void settings_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if(e.PropertyName == "PriceTick")
            {
                RaisePropertyChanged("TwoTick");
                RaisePropertyChanged("ThreeTick");
                RaisePropertyChanged("FourTick");
            }
        }

        private List<DirectionDependsItem> _directionDependsItems = new List<DirectionDependsItem>();
        public IEnumerable<DirectionDependsItem> DependsItems
        {
            get { return _directionDependsItems; }
        }

        private List<StopLossStrategyItem> _closeStrategyItems = new List<StopLossStrategyItem>();
        public IEnumerable<StopLossStrategyItem> CloseStrategyItems
        {
            get { return _closeStrategyItems; }
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

            _closeStrategyItems.Add(new StopLossStrategyItem
            {
                Value = entity.StopLossCloseMethods.BASED_ON_NEXT_QUOTE,
                DisplayName = "下一个报价"
            });

            _closeStrategyItems.Add(new StopLossStrategyItem
            {
                Value = entity.StopLossCloseMethods.BASED_ON_INPUT_LIMIT,
                DisplayName = "本次限价"
            });
        }
    }

    public class DirectionDependsItem
    {
        public entity.DirectionDepends Value { get; set; }
        public string DisplayName { get; set; }
    }

    public class StopLossStrategyItem
    {
        public entity.StopLossCloseMethods Value { get; set; }
        public string DisplayName { get; set; }
    }
}

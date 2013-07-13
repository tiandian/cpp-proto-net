using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ChangePositionSettingsVM : StrategySettingVM
    {
        #region CloseLeg
        private string _closeLeg;

        public string CloseLeg
        {
            get { return _closeLeg; }
            set
            {
                if (_closeLeg != value)
                {
                    _closeLeg = value;
                    RaisePropertyChanged("CloseLeg");
                }
            }
        }
        #endregion

        #region CloseLegSide
        private PTEntity.PosiDirectionType _side;

        public PTEntity.PosiDirectionType CloseLegSide
        {
            get { return _side; }
            set
            {
                if (_side != value)
                {
                    _side = value;
                    RaisePropertyChanged("CloseLegSide");
                }
            }
        }
        #endregion

        #region TriggerCondition
        private CompareCondition _triggerCond;

        public CompareCondition TriggerCondition
        {
            get { return _triggerCond; }
            set
            {
                if (_triggerCond != value)
                {
                    _triggerCond = value;
                    RaisePropertyChanged("TriggerCondition");
                }
            }
        }
        #endregion

        #region Threshold
        private double _threshold;

        public double Threshold
        {
            get { return _threshold; }
            set
            {
                if (_threshold != value)
                {
                    _threshold = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion

        public IEnumerable<CompareCondItem> ConditionItemsSource
        {
            get { return GreaterItemsSource.Concat(LessItemsSource); }
        }

        public IEnumerable<string> OptionalLegs { get; set; }

        protected override void OnSetPortfolio(Account.PortfolioVM portfVm)
        {
            OptionalLegs = from l in portfVm.Legs select l.Symbol;

            ChangePositionSetting settings = (ChangePositionSetting)portfVm.StrategySetting;
            this.CloseLeg = settings.CloseLeg;
            this.CloseLegSide = settings.CloseLegSide;
            this.TriggerCondition = settings.TriggerCondition;
            this.Threshold = settings.Threshold;
        }

        protected override void OnApplySetting()
        {
            ChangePositionSetting settings = (ChangePositionSetting)CurrentPortfolio.StrategySetting;

            settings.CloseLeg = this.CloseLeg;
            settings.CloseLegSide = this.CloseLegSide;
            settings.TriggerCondition = this.TriggerCondition;
            settings.Threshold = this.Threshold;

            base.OnApplySetting();
        }
    }
}

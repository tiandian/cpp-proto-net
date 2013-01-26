﻿using System;
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

        protected override void OnApplySetting()
        {
            ScalperSetting settings = (ScalperSetting)CurrentPortfolio.StrategySetting;

            settings.Threshold = this.Threshold;

            base.OnApplySetting();
        }

        protected override void OnSetPortfolio(Account.PortfolioVM portfVm)
        {
            ScalperSetting settings = (ScalperSetting)portfVm.StrategySetting;
            this.Threshold = settings.Threshold;
        }
    }
}

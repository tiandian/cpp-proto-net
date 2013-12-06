﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ASCTrendSettingsVM : StrategySettingVM
    {
        protected override StrategySetting CreateSettings()
        {
            return new ASCTrendStrategySettings();
        }
    }
}

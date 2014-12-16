using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ManualStrategySettingVM : StrategySettingVM
    {
        protected override StrategySetting CreateSettings()
        {
            return new ManualStrategySetting();
        }
    }
}

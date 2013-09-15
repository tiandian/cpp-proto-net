using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class MACDHistSlopeSettingsVM : StrategySettingVM
    {
        protected override StrategySetting CreateSettings()
        {
            return new MACDHistSlopeStrategySetting();
        }
    }
}

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
        public IEnumerable<CompareCondItem> ConditionItemsSource
        {
            get { return GreaterItemsSource.Concat(LessItemsSource); }
        }

        public IEnumerable<string> OptionalLegs { get; set; }

        protected override void OnSetPortfolio(Account.PortfolioVM portfVm)
        {
            OptionalLegs = from l in portfVm.Legs select l.Symbol;
        }

        protected override StrategySetting CreateSettings()
        {
            return new ChangePositionSetting();
        }
    }
}

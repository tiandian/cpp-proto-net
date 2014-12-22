using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Modules.Account;
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
        [ImportingConstructor]
        public ManualStrategySettingVM(IEventAggregator eventAggregator):base()
        {
            eventAggregator.GetEvent<ManualOpenDirectionChangeEvent>().Subscribe(OnManualOpenDirectionChange);
        }

        protected override StrategySetting CreateSettings()
        {
            return new ManualStrategySetting();
        }

        private void OnManualOpenDirectionChange(PortfolioVM portfVm)
        {
            if(CurrentPortfolio == portfVm)
            {
                OnResetSetting();
            }
        }
    }
}

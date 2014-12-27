using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Utils;
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
        private readonly RefDatItemsSource<PTEntity.StopPriceType, string> _stopPriceTypeItemsSource;

        public RefDatItemsSource<PTEntity.StopPriceType, string> StopPriceTypeItemsSource
        {
            get { return _stopPriceTypeItemsSource; }
        }

        [ImportingConstructor]
        public ManualStrategySettingVM(IEventAggregator eventAggregator):base()
        {
            _stopPriceTypeItemsSource = new RefDatItemsSource<PTEntity.StopPriceType, string>();
            _stopPriceTypeItemsSource.Add(new RefDataItem<PTEntity.StopPriceType, string> 
            {
                Value = PTEntity.StopPriceType.LOSS_STOP,
                DisplayText = "一般止损"
            });
            _stopPriceTypeItemsSource.Add(new RefDataItem<PTEntity.StopPriceType, string>
            {
                Value = PTEntity.StopPriceType.TRAILING_STOP,
                DisplayText = "追踪止损"
            });

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

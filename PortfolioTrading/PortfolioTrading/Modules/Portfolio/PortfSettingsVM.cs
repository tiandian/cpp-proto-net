using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Modules.Portfolio.Strategy;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Portfolio
{
    [Export]
    public class PortfSettingsVM : NotificationObject
    {
        private List<DirectionItem> _directionItems = new List<DirectionItem>();
        private PortfolioVM _lastPortfVm;

        [ImportingConstructor]
        public PortfSettingsVM(IEventAggregator evtAgg)
        {
            evtAgg.GetEvent<PortfolioSelectedEvent>().Subscribe(SetPortfolio);

            _directionItems.Add(new DirectionItem 
            { 
                Direction = entity.PosiDirectionType.LONG,
                DisplayText = "多头"
            });

            _directionItems.Add(new DirectionItem 
            {
                Direction = entity.PosiDirectionType.SHORT,
                DisplayText = "空头"
            });

            ApplyCommand = new DelegateCommand(OnApplySetting);
            ResetCommand = new DelegateCommand(OnResetSetting);
        }

        public IEnumerable<DirectionItem> DirectionItemsSource
        {
            get { return _directionItems; }
        }

        public DelegateCommand ApplyCommand { get; private set; }
        public DelegateCommand ResetCommand { get; private set; }

        private void OnApplySetting()
        {
            ArbitrageStrategySetting strategySettings = _lastPortfVm.StrategySetting as ArbitrageStrategySetting;
            strategySettings.Direction = this.PositionDirection;
            strategySettings.OpenThreshold = this.OpenThreshold;
            strategySettings.StopGainThreshold = this.StopGainThreshold;
            strategySettings.StopLossThreshold = this.StopLossThreshold;

            _lastPortfVm.ApplyStrategySettings();
        }

        private void OnResetSetting()
        {
            SetPortfolio(_lastPortfVm);
        }

        public void SetPortfolio(PortfolioVM portfVm)
        {
            this.PortfolioID = portfVm.Id;
            if (portfVm.StrategySetting.Name == StrategySetting.ArbitrageStrategyName)
            {
                ArbitrageStrategySetting strategySettings = portfVm.StrategySetting as ArbitrageStrategySetting;
                this.PositionDirection = strategySettings.Direction;
                this.OpenThreshold = strategySettings.OpenThreshold;
                this.StopGainThreshold = strategySettings.StopGainThreshold;
                this.StopLossThreshold = strategySettings.StopLossThreshold;

                this._lastPortfVm = portfVm;
            }
            
        }

        #region PortfolioID
        private string _portfId;

        public string PortfolioID
        {
            get { return _portfId; }
            set
            {
                if (_portfId != value)
                {
                    _portfId = value;
                    RaisePropertyChanged("PortfolioID");
                }
            }
        }
        #endregion

        #region PositionDirection
        private entity.PosiDirectionType _direction;

        public entity.PosiDirectionType PositionDirection
        {
            get { return _direction; }
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    RaisePropertyChanged("PositionDirection");
                }
            }
        }
        #endregion


        #region OpenThreshold
        private double _openThreshold;

        public double OpenThreshold
        {
            get { return _openThreshold; }
            set
            {
                if (_openThreshold != value)
                {
                    _openThreshold = value;
                    RaisePropertyChanged("OpenThreshold");
                }
            }
        }
        #endregion

        #region StopGainThreshold
        private double _stopGainThreshold;

        public double StopGainThreshold
        {
            get { return _stopGainThreshold; }
            set
            {
                if (_stopGainThreshold != value)
                {
                    _stopGainThreshold = value;
                    RaisePropertyChanged("StopGainThreshold");
                }
            }
        }
        #endregion

        #region StopLossThreshold
        private double _stopLossThreshold;

        public double StopLossThreshold
        {
            get { return _stopLossThreshold; }
            set
            {
                if (_stopLossThreshold != value)
                {
                    _stopLossThreshold = value;
                    RaisePropertyChanged("StopLossThreshold");
                }
            }
        }
        #endregion

    }

    public class DirectionItem
    {
        public entity.PosiDirectionType Direction { get; set; }
        public string DisplayText { get; set; }
    }
}

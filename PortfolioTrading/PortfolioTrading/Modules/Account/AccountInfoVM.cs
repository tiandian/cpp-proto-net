using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Events;
using Microsoft.Practices.Prism.Events;
using System.Threading;

namespace PortfolioTrading.Modules.Account
{
    [Export]
    public class AccountInfoVM : NotificationObject
    {
        #region AccountId
        private string _acctId;

        public string AccountId
        {
            get { return _acctId; }
            set
            {
                if (_acctId != value)
                {
                    _acctId = value;
                    RaisePropertyChanged("AccountId");
                }
            }
        }
        #endregion

        #region Available 可用资金
        private double _available;

        public double Available
        {
            get { return _available; }
            set
            {
                if (_available != value)
                {
                    _available = value;
                    RaisePropertyChanged("Available");
                }
            }
        }
        #endregion

        #region Balance 结算准备金
        private double _balance;

        public double Balance
        {
            get { return _balance; }
            set
            {
                if (_balance != value)
                {
                    _balance = value;
                    RaisePropertyChanged("Balance");
                }
            }
        }
        #endregion

        #region FrozenMargin 占用保证金

        private double _frozenMargin;

        public double FrozenMargin
        {
            get { return _frozenMargin; }
            set
            {
                if (_frozenMargin != value)
                {
                    _frozenMargin = value;
                    RaisePropertyChanged("FrozenMargin");
                }
            }
        }
        #endregion

        #region FrozenCash 下单冻结
        private double _frozenCash;

        public double FrozenCash
        {
            get { return _frozenCash; }
            set
            {
                if (_frozenCash != value)
                {
                    _frozenCash = value;
                    RaisePropertyChanged("FrozenCash");
                }
            }
        }
        #endregion

        #region CloseProfit 平仓盈亏
        private double _closeProfit;

        public double CloseProfit
        {
            get { return _closeProfit; }
            set
            {
                if (_closeProfit != value)
                {
                    _closeProfit = value;
                    RaisePropertyChanged("CloseProfit");
                }
            }
        }
        #endregion

        #region PositionProfit 持仓盈亏
        private double _positionProfit;

        public double PositionProfit
        {
            get { return _positionProfit; }
            set
            {
                if (_positionProfit != value)
                {
                    _positionProfit = value;
                    RaisePropertyChanged("PositionProfit");
                }
            }
        }
        #endregion

        #region CurrMargin 当前保证金
        private double _currMargin;

        public double CurrMargin
        {
            get { return _currMargin; }
            set
            {
                if (_currMargin != value)
                {
                    _currMargin = value;
                    RaisePropertyChanged("CurrMargin");
                }
            }
        }
        #endregion

        #region Commission 手续费
        private double _commission;

        public double Commission
        {
            get { return _commission; }
            set
            {
                if (_commission != value)
                {
                    _commission = value;
                    RaisePropertyChanged("Commission");
                }
            }
        }
        #endregion

        #region UpdateTime
        private DateTime _updateTime;

        public DateTime UpdateTime
        {
            get { return _updateTime; }
            set
            {
                if (_updateTime != value)
                {
                    _updateTime = value;
                    RaisePropertyChanged("UpdateTime");
                }
            }
        }
        #endregion

        public DelegateCommand QueryAccountCommand { get; private set; }
        public DelegateCommand QueryPositionCommand { get; private set; }

        private AccountVM CurrentAccount { get; set; }
        private IEventAggregator EventAggregator { get; set; }

        [ImportingConstructor]
        public AccountInfoVM(IEventAggregator evtAgg)
        {
            QueryAccountCommand = new DelegateCommand(OnQueryAccount);
            QueryPositionCommand = new DelegateCommand(OnQueryPosition);
            evtAgg.GetEvent<AccountSelectedEvent>().Subscribe(OnAccountSelected, ThreadOption.UIThread);
            EventAggregator = evtAgg;
        }

        public void OnAccountSelected(AccountVM accountVm)
        {
            AccountId = accountVm.InvestorId;
            CurrentAccount = accountVm;
            OnQueryAccount();
        }

        private void OnQueryPosition()
        {
            EventAggregator.GetEvent<QueryPositionEvent>().Publish(CurrentAccount);
        }

        private void OnQueryAccount()
        {
            SynchronizationContext uiContext = SynchronizationContext.Current;
            QueryAccountInfoWithContext(uiContext);
        }

        private void QueryAccountInfoWithContext(SynchronizationContext uiContext)
        {
            if (!CurrentAccount.IsConnected) return;

            CurrentAccount.QueryAccountInfo(ai =>
            {
                if (uiContext != null)
                {
                    uiContext.Post(o => UpdateAccoutnInfo(ai), null);
                }
            });
        }

        private void UpdateAccoutnInfo(trade.AccountInfo acctInfoData)
        {
            Available = acctInfoData.Available;
            Balance = acctInfoData.Balance;
            CurrMargin = acctInfoData.CurrMargin;
            Commission = acctInfoData.Commission;
            FrozenMargin = acctInfoData.FrozenMargin;
            FrozenCash = acctInfoData.FrozenCash;
            CloseProfit = acctInfoData.CloseProfit;
            PositionProfit = acctInfoData.PositionProfit;
            UpdateTime = DateTime.Now;
        }
    }
}

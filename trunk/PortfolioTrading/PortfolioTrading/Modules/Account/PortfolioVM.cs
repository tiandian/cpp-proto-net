using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using System.Xml.Linq;
using System.Windows.Input;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Utils;
using PortfolioTrading.Events;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.ServiceLocation;
using PortfolioTrading.Modules.Portfolio.Strategy;
using PortfolioTrading.Modules.Portfolio;

namespace PortfolioTrading.Modules.Account
{
    public class PortfolioVM : NotificationObject
    {
        private readonly AccountVM _accountVm;
        private ObservableCollection<LegVM> _legs = new ObservableCollection<LegVM>();

        public PortfolioVM(AccountVM accountVm)
        {
            _accountVm = accountVm;
            OpenPositionCommand = new DelegateCommand(OnOpenPosition);
            OpenQtyPositionCommand = new DelegateCommand(OnOpenQtyPosition);
            ClosePositionCommand = new DelegateCommand(OnClosePosition);
            CloseQtyPositionCommand = new DelegateCommand(OnCloseQtyPosition);
            StartCommand = new DelegateCommand(OnStart);
            StopCommand = new DelegateCommand(OnStop);
        }

        public AccountVM Account { get { return _accountVm; } }

        public string AccountId
        {
            get { return _accountVm.Id; }
        }

        private bool IsLoading { get; set; }

        #region Id
        private string _id;

        public string Id
        {
            get { return _id; }
            set
            {
                if (_id != value)
                {
                    _id = value;
                    RaisePropertyChanged("Id");
                }
            }
        }
        #endregion

        #region Quantity
        private int _qty;

        public int Quantity
        {
            get { return _qty; }
            set
            {
                if (_qty != value)
                {
                    _qty = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

        #region Diff
        private decimal _diff;

        public decimal Diff
        {
            get { return _diff; }
            set
            {
                if (_diff != value)
                {
                    _diff = value;
                    RaisePropertyChanged("Diff");
                }
            }
        }
        #endregion

        #region LongDiff
        private decimal _longDiff;

        public decimal LongDiff
        {
            get { return _longDiff; }
            set
            {
                if (_longDiff != value)
                {
                    _longDiff = value;
                    RaisePropertyChanged("LongDiff");
                }
            }
        }
        #endregion

        #region ShortDiff
        private decimal _shortDiff;

        public decimal ShortDiff
        {
            get { return _shortDiff; }
            set
            {
                if (_shortDiff != value)
                {
                    _shortDiff = value;
                    RaisePropertyChanged("ShortDiff");
                }
            }
        }
        #endregion

        #region LongSize
        private int _longSize;

        public int LongSize
        {
            get { return _longSize; }
            set
            {
                if (_longSize != value)
                {
                    _longSize = value;
                    RaisePropertyChanged("LongSize");
                }
            }
        }
        #endregion

        #region ShortSize
        private int _shortSize;

        public int ShortSize
        {
            get { return _shortSize; }
            set
            {
                if (_shortSize != value)
                {
                    _shortSize = value;
                    RaisePropertyChanged("ShortSize");
                }
            }
        }
        #endregion

        #region AutoOpen
        private bool _autoOpen;

        public bool AutoOpen
        {
            get { return _autoOpen; }
            set
            {
                if (_autoOpen != value)
                {
                    _autoOpen = value;
                    RaisePropertyChanged("AutoOpen");
                    OnSwitchChanged();
                }
            }
        }
        #endregion

        #region AutoStopGain
        private bool _autoStopGain;

        public bool AutoStopGain
        {
            get { return _autoStopGain; }
            set
            {
                if (_autoStopGain != value)
                {
                    _autoStopGain = value;
                    RaisePropertyChanged("AutoStopGain");
                    OnSwitchChanged();
                }
            }
        }
        #endregion

        #region AutoStopLoss
        private bool _autoStopLoss;

        public bool AutoStopLoss
        {
            get { return _autoStopLoss; }
            set
            {
                if (_autoStopLoss != value)
                {
                    _autoStopLoss = value;
                    RaisePropertyChanged("AutoStopLoss");
                    OnSwitchChanged();
                }
            }
        }
        #endregion

        #region AutoTracking
        private bool _autoTracking = true;

        public bool AutoTracking
        {
            get { return _autoTracking; }
            set
            {
                if (_autoTracking != value)
                {
                    _autoTracking = value;
                    RaisePropertyChanged("AutoTracking");
                    OnSwitchChanged();
                }
            }
        }
        #endregion

        #region EnablePrefer
        private bool _enablePrefer;

        public bool EnablePrefer
        {
            get { return _enablePrefer; }
            set
            {
                if (_enablePrefer != value)
                {
                    _enablePrefer = value;
                    RaisePropertyChanged("EnablePrefer");
                    OnSwitchChanged();
                }
            }
        }
        #endregion

        #region OpenTimes
        private int _openTimes;

        public int OpenTimes
        {
            get { return _openTimes; }
            set
            {
                if (_openTimes != value)
                {
                    _openTimes = value;
                    RaisePropertyChanged("OpenTimes");
                }
            }
        }
        #endregion

        #region DoneTimes
        private int _doneTimes;

        public int DoneTimes
        {
            get { return _doneTimes; }
            set
            {
                if (_doneTimes != value)
                {
                    _doneTimes = value;
                    RaisePropertyChanged("DoneTimes");
                }
            }
        }
        #endregion

        #region Position
        private int _position;

        public int Position
        {
            get { return _position; }
            set
            {
                if (_position != value)
                {
                    _position = value;
                    RaisePropertyChanged("Position");
                }
            }
        }
        #endregion

        #region MaxPosition
        private int _maxPosition;

        public int MaxPosition
        {
            get { return _maxPosition; }
            set
            {
                if (_maxPosition != value)
                {
                    _maxPosition = value;
                    RaisePropertyChanged("MaxPosition");
                }
            }
        }
        #endregion


        #region Gain
        private double _gain;

        public double Gain
        {
            get { return _gain; }
            set
            {
                if (_gain != value)
                {
                    _gain = value;
                    RaisePropertyChanged("Gain");
                }
            }
        }
        #endregion

        #region IsRunning
        private bool _isRunning;

        public bool IsRunning
        {
            get { return _isRunning; }
            set
            {
                if (_isRunning != value)
                {
                    _isRunning = value;
                    RaisePropertyChanged("IsRunning");
                }
            }
        }
        #endregion


        public StrategySetting StrategySetting { get; set; }

        public string DisplayText
        {
            get
            {
                StringBuilder txt = new StringBuilder();
                txt.AppendFormat("{0}: ", Id);
                for (int i = 0; i < _legs.Count; ++i)
                {
                    if (i > 0)
                        txt.Append(" - ");

                    var l = _legs[i];
                    txt.AppendFormat("{0}({1})", l.Name, 
                        l.Side == entity.PosiDirectionType.LONG ? "多" : "空");
                }

                return txt.ToString();
            }
        }

        public ICommand OpenPositionCommand { get; private set; }
        public ICommand OpenQtyPositionCommand { get; private set; }
        public ICommand ClosePositionCommand { get; private set; }
        public ICommand CloseQtyPositionCommand { get; private set; }

        public ICommand StartCommand { get; private set; }
        public ICommand StopCommand { get; private set; }
        
        public IEnumerable<LegVM> Legs
        {
            get { return _legs; }
        }

        public void AddLeg(LegVM leg)
        {
            _legs.Add(leg);
            leg.PortfolioId = Id;
            leg.OnIsPreferredChanged += new Action<LegVM, bool>(OnModifyPreferredLeg);
        }

        void OnModifyPreferredLeg(LegVM leg, bool obj)
        {
            if(this._accountVm.IsConnected)
                this._accountVm.Host.PortfSetPreferredLeg(this.Id, leg.Name);

            _accountVm.PublishChanged();
        }

        public int LegCount
        {
            get
            {
                return _legs.Count;
            }
        }

        public LegVM GetLeg(int idx)
        {
            if (idx < _legs.Count && idx > -1)
            {
                return _legs[idx];
            }

            return null;
        }

        public static PortfolioVM Load(AccountVM acct, XElement xmlElement)
        {
            PortfolioVM portf = new PortfolioVM(acct);
            portf.IsLoading = true;
            XAttribute attr = xmlElement.Attribute("id");
            if (attr != null)
                portf.Id = attr.Value;

            attr = xmlElement.Attribute("quantity");
            if (attr != null)
            {
                portf.Quantity = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("maxPosition");
            if (attr != null)
            {
                portf.MaxPosition = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("autoOpen");
            if (attr != null)
            {
                portf.AutoOpen = attr.Value == bool.TrueString;
            }

            attr = xmlElement.Attribute("autoStopGain");
            if (attr != null)
            {
                portf.AutoStopGain = attr.Value == bool.TrueString;
            }

            attr = xmlElement.Attribute("autoStopLoss");
            if (attr != null)
            {
                portf.AutoStopLoss = attr.Value == bool.TrueString;
            }

            attr = xmlElement.Attribute("autoTracking");
            if (attr != null)
            {
                portf.AutoTracking = attr.Value == bool.TrueString;
            }

            attr = xmlElement.Attribute("enablePrefer");
            if (attr != null)
            {
                portf.EnablePrefer = attr.Value == bool.TrueString;
            }

            foreach (var legElem in xmlElement.Element("legs").Elements("leg"))
            {
                LegVM legVm = LegVM.Load(legElem);
                portf.AddLeg(legVm);
            }
            
            XElement xmlSetting = xmlElement.Element("setting");
            string strategyName = xmlSetting.Attribute("name").Value;
            string strategyXmlText = xmlSetting.Value;

            portf.StrategySetting = StrategySetting.Load(strategyName, strategyXmlText);
            portf.IsLoading = false;
            return portf;
        }

        public XElement Persist()
        {
            XElement elem = new XElement("portfolio");
            elem.Add(new XAttribute("id", _id));
            elem.Add(new XAttribute("quantity", _qty));
            elem.Add(new XAttribute("maxPosition", _maxPosition));
            elem.Add(new XAttribute("autoOpen", _autoOpen.ToString()));
            elem.Add(new XAttribute("autoStopGain", _autoStopGain.ToString()));
            elem.Add(new XAttribute("autoStopLoss", _autoStopLoss.ToString()));
            elem.Add(new XAttribute("autoTracking", _autoTracking.ToString()));
            elem.Add(new XAttribute("enablePrefer", _enablePrefer.ToString()));
            

            XElement elemLegs = new XElement("legs");
            foreach (var l in _legs)
            {
                elemLegs.Add(l.Persist());
            }
            elem.Add(elemLegs);

            XElement settingElem = new XElement("setting", new XAttribute("name", StrategySetting.Name));
            settingElem.Add(new XCData(StrategySetting.Persist()));

            elem.Add(settingElem);

            return elem;
        }

        public entity.PortfolioItem GetEntity()
        {
            entity.PortfolioItem portfolioItem = new entity.PortfolioItem();
            portfolioItem.ID = Id;
            portfolioItem.AutoOpen = AutoOpen;
            portfolioItem.AutoStopGain = AutoStopGain;
            portfolioItem.AutoStopLoss = AutoStopLoss;
            portfolioItem.AutoTrack = AutoTracking;
            portfolioItem.EnablePrefer = EnablePrefer;
            portfolioItem.Quantity = Quantity;
            portfolioItem.MaxPosition = MaxPosition;

            foreach (var legVm in _legs)
            {
                entity.LegItem leg = new entity.LegItem();
                leg.Symbol = legVm.Symbol;
                leg.Side = legVm.Side;
                leg.Ratio = legVm.Ratio;
                leg.IsPreferred = legVm.IsPreferred;
                portfolioItem.Legs.Add(leg);
            }

            portfolioItem.StrategyName = StrategySetting.Name;
            portfolioItem.StrategyData = StrategySetting.Serialize();

            return portfolioItem;
        }

        public void Update(entity.PortfolioItem item)
        {
            Diff = ToDecimal(item.Diff);
            LongDiff = ToDecimal(item.LongDiff);
            ShortDiff = ToDecimal(item.ShortDiff);
            LongSize = item.LongSize;
            ShortSize = item.ShortSize;
            OpenTimes = item.OpenTimes;
            DoneTimes = item.CloseTimes;
            Position = item.CurrentPosition;
            Gain = item.Profit;

            IsRunning = item.StrategyRunning;

            for (int i = 0; i < item.Legs.Count; ++i )
            {
                _legs[i].Update(item.Legs[i]);
            }
        }

        private static decimal ToDecimal(double val)
        {
            return val == double.MaxValue || val == double.MinValue ? 
                0 : (decimal)Math.Round(val, 2);
        }

        private void OnOpenPosition()
        {
            _accountVm.Host.PorfOpenPosition(Id, Quantity);
            EventLogger.Write("{0} 开仓组合 {1}, 数量 {2}", _accountVm.InvestorId, DisplayText, Quantity);
        }

        private void OnOpenQtyPosition()
        {
            var viewModel = new SpecifyQuantityViewModel(SpecifyQuantityViewModel.SpecifyQtyMode.Open);
            ChangeSpecifiedQtyDlg dlg = new ChangeSpecifiedQtyDlg(viewModel);
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            bool? ret = dlg.ShowDialog();
            if(ret ?? false)
            {
                int qty = viewModel.Quantity;
                _accountVm.Host.PorfOpenPosition(Id, qty);
                EventLogger.Write("{0} 开仓组合 {1}, 数量 {2}", _accountVm.InvestorId, DisplayText, qty);
            }
        }

        private void OnClosePosition()
        {
            _accountVm.Host.PorfClosePosition(Id, 0);
            EventLogger.Write("{0} 平仓组合 {1}", _accountVm.InvestorId, DisplayText);
        }

        private void OnCloseQtyPosition()
        {
            var viewModel = new SpecifyQuantityViewModel(SpecifyQuantityViewModel.SpecifyQtyMode.Close);
            ChangeSpecifiedQtyDlg dlg = new ChangeSpecifiedQtyDlg(viewModel);
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            bool? ret = dlg.ShowDialog();
            if (ret ?? false)
            {
                int qty = viewModel.Quantity;
                _accountVm.Host.PorfClosePosition(Id, qty);
                EventLogger.Write("{0} 平仓组合 {1}, 数量 {2}", _accountVm.InvestorId, DisplayText, qty);
            }
        }

        private void OnStart()
        {
            IsRunning = true;
            OnRunningChanged();
        }

        private void OnStop()
        {
            IsRunning = false;
            OnRunningChanged();
        }

        private void OnRunningChanged()
        {
            if (_accountVm.IsConnected)
            {
                _accountVm.Host.PortfEnableStrategy(Id, IsRunning);
            }
        }

        private void OnSwitchChanged()
        {
            if (_accountVm.IsConnected)
            {
                _accountVm.Host.PortfTurnSwitches(this.Id, AutoOpen, AutoStopGain, AutoStopLoss,
                    AutoTracking, EnablePrefer);
            }

            if(!IsLoading)
                _accountVm.PublishChanged();
        }

        public void ApplyStrategySettings()
        {
            _accountVm.Host.PortfApplyStrategySettings(this.Id,
                StrategySetting.Name, StrategySetting.Serialize());

            _accountVm.PublishChanged();
        }

    }
}

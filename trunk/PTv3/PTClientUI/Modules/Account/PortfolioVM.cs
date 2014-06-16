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
            ModifyQtyCommand = new DelegateCommand(OnModifyQuantity);
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
        private int _qty = 1;

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
        
        #region AR_BollTop
        private double _ar_bollTop;

        public double AR_BollTop
        {
            get { return _ar_bollTop; }
            set
            {
                if (_ar_bollTop!= value)
                {
                    _ar_bollTop= value;
                    RaisePropertyChanged("AR_BollTop");
                }
            }
        }
        #endregion

        #region AR_BollBottom
        private double _ar_bollBottom;

        public double AR_BollBottom
        {
            get { return _ar_bollBottom; }
            set
            {
                if (_ar_bollBottom!= value)
                {
                    _ar_bollBottom= value;
                    RaisePropertyChanged("AR_BollBottom");
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

        #region AvgCost
        private double _avgCost;

        public double AvgCost
        {
            get { return _avgCost; }
            set
            {
                if (_avgCost != value)
                {
                    _avgCost = value;
                    RaisePropertyChanged("AvgCost");
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

        #region CancelVolume
        private int _cancelVol;

        public int CancelVolume
        {
            get { return _cancelVol; }
            set
            {
                if (_cancelVol != value)
                {
                    _cancelVol = value;
                    RaisePropertyChanged("CancelVolume");
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

        #region MaxOpenPerStart
        private int _maxOpenPerStart = 200;

        public int MaxOpenPerStart
        {
            get { return _maxOpenPerStart; }
            set
            {
                if (_maxOpenPerStart != value)
                {
                    _maxOpenPerStart = value;
                    RaisePropertyChanged("MaxOpenPerStart");
                }
            }
        }
        #endregion

        #region MaxCancel
        private int _maxCancel = 450;

        public int MaxCancel
        {
            get { return _maxCancel; }
            set
            {
                if (_maxCancel != value)
                {
                    _maxCancel = value;
                    RaisePropertyChanged("MaxCancel");
                }
            }
        }
        #endregion

        #region TotalOpenLimit
        private int _totalOpenLimit = 450;

        public int TotalOpenLimit
        {
            get { return _totalOpenLimit; }
            set
            {
                if (_totalOpenLimit != value)
                {
                    _totalOpenLimit = value;
                    RaisePropertyChanged("TotalOpenLimit");
                }
            }
        }
        #endregion

        #region EndTimePointsExpr
        private string _endTimePointsExpr = string.Empty;

        public string EndTimePointsExpr
        {
            get { return _endTimePointsExpr; }
            set
            {
                if (_endTimePointsExpr != value)
                {
                    _endTimePointsExpr = value;
                    RaisePropertyChanged("EndTimePointsExpr");
                }
            }
        }
        #endregion

#region MACD Hist Slope strategy updating fields

        #region FastAngle
        private decimal _fastAngle;

        public decimal FastAngle
        {
            get { return _fastAngle; }
            set
            {
                if (_fastAngle != value)
                {
                    _fastAngle = value;
                    RaisePropertyChanged("FastAngle");
                }
            }
        }
        #endregion

        #region SlowAngle
        private decimal _slowAngle;

        public decimal SlowAngle
        {
            get { return _slowAngle; }
            set
            {
                if (_slowAngle != value)
                {
                    _slowAngle = value;
                    RaisePropertyChanged("SlowAngle");
                }
            }
        }
        #endregion

        #region FastMacdHist
        private decimal _fastMacdHist;

        public decimal FastMacdHist
        {
            get { return _fastMacdHist; }
            set
            {
                if (_fastMacdHist != value)
                {
                    _fastMacdHist = value;
                    RaisePropertyChanged("FastMacdHist");
                }
            }
        }
        #endregion

        #region FastMacdHistDiff
        private decimal _fastMacdHistDiff;

        public decimal FastMacdHistDiff
        {
            get { return _fastMacdHistDiff; }
            set
            {
                if (_fastMacdHistDiff != value)
                {
                    _fastMacdHistDiff = value;
                    RaisePropertyChanged("FastMacdHistDiff");
                }
            }
        }
        #endregion

        #region SlowMacdHist
        private decimal _slowMacdHist;

        public decimal SlowMacdHist
        {
            get { return _slowMacdHist; }
            set
            {
                if (_slowMacdHist != value)
                {
                    _slowMacdHist = value;
                    RaisePropertyChanged("SlowMacdHist");
                }
            }
        }
        #endregion

        #region SlowMacdHistDiff
        private decimal _slowMacdHistDiff;

        public decimal SlowMacdHistDiff
        {
            get { return _slowMacdHistDiff; }
            set
            {
                if (_slowMacdHistDiff != value)
                {
                    _slowMacdHistDiff = value;
                    RaisePropertyChanged("SlowMacdHistDiff");
                }
            }
        }
        #endregion

        #region FastSlopeDirection
        private PTEntity.SlopeDirection _fastSlopeDirection;

        public PTEntity.SlopeDirection FastSlopeDirection
        {
            get { return _fastSlopeDirection; }
            set
            {
                if (_fastSlopeDirection != value)
                {
                    _fastSlopeDirection = value;
                    RaisePropertyChanged("FastSlopeDirection");
                }
            }
        }
        #endregion

        #region SlowSlopeDirection
        private PTEntity.SlopeDirection _slowSlopeDirection;

        public PTEntity.SlopeDirection SlowSlopeDirection
        {
            get { return _slowSlopeDirection; }
            set
            {
                if (_slowSlopeDirection != value)
                {
                    _slowSlopeDirection = value;
                    RaisePropertyChanged("SlowSlopeDirection");
                }
            }
        }
        #endregion
        
#endregion

#region WMA Trend strategy updating fields

        #region FastLine
        private decimal _fastLine;

        public decimal FastLine
        {
            get { return _fastLine; }
            set
            {
                if (_fastLine != value)
                {
                    _fastLine = value;
                    RaisePropertyChanged("FastLine");
                }
            }
        }
        #endregion

        #region SlowLine
        private decimal _slowLine;

        public decimal SlowLine
        {
            get { return _slowLine; }
            set
            {
                if (_slowLine != value)
                {
                    _slowLine = value;
                    RaisePropertyChanged("SlowLine");
                }
            }
        }
        #endregion

#endregion

#region Liner Regression strategy updating fields

        #region WeightMidPoint
        private decimal _weightMidPoint;

        public decimal WeightMidPoint
        {
            get { return _weightMidPoint; }
            set
            {
                if (_weightMidPoint != value)
                {
                    _weightMidPoint = value;
                    RaisePropertyChanged("WeightMidPoint");
                }
            }
        }
        #endregion

        #region LinerRegressionAngle
        private decimal _linerRegAngle;

        public decimal LinerRegressionAngle
        {
            get { return _linerRegAngle; }
            set
            {
                if (_linerRegAngle != value)
                {
                    _linerRegAngle = value;
                    RaisePropertyChanged("LinerRegressionAngle");
                }
            }
        }
        #endregion

#endregion

#region ASC Trend updating fields

        #region AS_WilliamsR
        private decimal _asWilliamsR;

        public decimal AS_WilliamsR
        {
            get { return _asWilliamsR; }
            set
            {
                if (_asWilliamsR != value)
                {
                    _asWilliamsR = value;
                    RaisePropertyChanged("AS_WilliamsR");
                }
            }
        }
        #endregion

        #region AS_StopPx
        private decimal _asStopPx;

        public decimal AS_StopPx
        {
            get { return _asStopPx; }
            set
            {
                if (_asStopPx != value)
                {
                    _asStopPx = value;
                    RaisePropertyChanged("AS_StopPx");
                }
            }
        }
        #endregion

        #region AS_DonchianHi
        private decimal _asDonchianHi;

        public decimal AS_DonchianHi
        {
            get { return _asDonchianHi; }
            set
            {
                if (_asDonchianHi != value)
                {
                    _asDonchianHi = value;
                    RaisePropertyChanged("AS_DonchianHi");
                }
            }
        }
        #endregion

        #region AS_DonchianLo
        private decimal _asDonchianLo;

        public decimal AS_DonchianLo
        {
            get { return _asDonchianLo; }
            set
            {
                if (_asDonchianLo != value)
                {
                    _asDonchianLo = value;
                    RaisePropertyChanged("AS_DonchianLo");
                }
            }
        }
        #endregion

#endregion

#region Range Trend updating fields

        #region RT_UpperBoundOpen
        private decimal _rtUpperBoundOpen;

        public decimal RT_UpperBoundOpen
        {
            get { return _rtUpperBoundOpen; }
            set
            {
                if (_rtUpperBoundOpen != value)
                {
                    _rtUpperBoundOpen = value;
                    RaisePropertyChanged("RT_UpperBoundOpen");
                }
            }
        }
        #endregion

        #region RT_LowerBoundOpen
        private decimal _rtLowerBoundOpen;

        public decimal RT_LowerBoundOpen
        {
            get { return _rtLowerBoundOpen; }
            set
            {
                if (_rtLowerBoundOpen != value)
                {
                    _rtLowerBoundOpen = value;
                    RaisePropertyChanged("RT_LowerBoundOpen");
                }
            }
        }
        #endregion

        #region RT_UpperBoundClose
        private decimal _rtUpperBoundClose;

        public decimal RT_UpperBoundClose
        {
            get { return _rtUpperBoundClose; }
            set
            {
                if (_rtUpperBoundClose != value)
                {
                    _rtUpperBoundClose = value;
                    RaisePropertyChanged("RT_UpperBoundClose");
                }
            }
        }
        #endregion

        #region RT_LowerBoundClose
        private decimal _rtLowerBoundClose;

        public decimal RT_LowerBoundClose
        {
            get { return _rtLowerBoundClose; }
            set
            {
                if (_rtLowerBoundClose != value)
                {
                    _rtLowerBoundClose = value;
                    RaisePropertyChanged("RT_LowerBoundClose");
                }
            }
        }
        #endregion

        #region RT_LastCostPx
        private decimal _rtLastCostPx;

        public decimal RT_LastCostPx
        {
            get { return _rtLastCostPx; }
            set
            {
                if (_rtLastCostPx != value)
                {
                    _rtLastCostPx = value;
                    RaisePropertyChanged("RT_LastCostPx");
                }
            }
        }
        #endregion

        #region RT_RecentStopLossPx
        private decimal _rtRecentStopLossPx;

        public decimal RT_RecentStopLossPx
        {
            get { return _rtRecentStopLossPx; }
            set
            {
                if (_rtRecentStopLossPx != value)
                {
                    _rtRecentStopLossPx = value;
                    RaisePropertyChanged("RT_RecentStopLossPx");
                }
            }
        }
        #endregion

#endregion

        public StrategySetting StrategySetting { get; set; }

        public string DisplayText
        {
            get
            {
                StringBuilder txt = new StringBuilder();
                txt.AppendFormat("[{0}] ", StrategySetting.GetDisplayStrategyName(StrategySetting.Name));

                txt.AppendFormat("{0}: ", Id);
                for (int i = 0; i < _legs.Count; ++i)
                {
                    if (i > 0)
                        txt.Append(" - ");

                    var l = _legs[i];
                    txt.AppendFormat("{0}({1})", l.Name, 
                        l.Side == PTEntity.PosiDirectionType.LONG ? "多" : "空");
                }

                return txt.ToString();
            }
        }

        public ICommand OpenPositionCommand { get; private set; }
        public ICommand OpenQtyPositionCommand { get; private set; }
        public ICommand ClosePositionCommand { get; private set; }
        public ICommand CloseQtyPositionCommand { get; private set; }
        public ICommand ModifyQtyCommand { get; private set; }

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
            
            attr = xmlElement.Attribute("currentPosition");
            if (attr != null)
            {
                int currPos = int.Parse(attr.Value);
                portf.Position = currPos;
                portf.OpenTimes = currPos;
            }
            
            attr = xmlElement.Attribute("avgCost");
            if(attr != null)
            {
                double avgCost = double.Parse(attr.Value);
                portf.AvgCost = avgCost;
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

            attr = xmlElement.Attribute("maxOpenPerStart");
            if (attr != null)
            {
                portf.MaxOpenPerStart = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("maxCancel");
            if (attr != null)
            {
                portf.MaxCancel = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("totalOpenLimit");
            if (attr != null)
            {
                portf.TotalOpenLimit = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("endTimePointsExpr");
            if (attr != null)
            {
                portf.EndTimePointsExpr = attr.Value;
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
            elem.Add(new XAttribute("currentPosition", _position));
            elem.Add(new XAttribute("avgCost", _avgCost.ToString("F2")));
            elem.Add(new XAttribute("autoOpen", _autoOpen.ToString()));
            elem.Add(new XAttribute("autoStopGain", _autoStopGain.ToString()));
            elem.Add(new XAttribute("autoStopLoss", _autoStopLoss.ToString()));
            elem.Add(new XAttribute("autoTracking", _autoTracking.ToString()));
            elem.Add(new XAttribute("enablePrefer", _enablePrefer.ToString()));
            elem.Add(new XAttribute("maxOpenPerStart", _maxOpenPerStart));
            elem.Add(new XAttribute("maxCancel", _maxCancel));
            elem.Add(new XAttribute("totalOpenLimit", _totalOpenLimit));
            elem.Add(new XAttribute("endTimePointsExpr", _endTimePointsExpr));

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

        public PTEntity.PortfolioItem GetEntity()
        {
            PTEntity.PortfolioItem portfolioItem = new PTEntity.PortfolioItem();
            portfolioItem.ID = Id;
            portfolioItem.Quantity = Quantity;
            portfolioItem.MaxOpenPerStart = MaxOpenPerStart;
            portfolioItem.MaxCancel = MaxCancel;
            portfolioItem.TotalOpenLimit = TotalOpenLimit;
            
            foreach (var legVm in _legs)
            {
                PTEntity.LegItem leg = new PTEntity.LegItem();
                leg.Symbol = legVm.Symbol;
                leg.Side = (PTEntity.PosiDirectionType)legVm.Side;
                leg.Ratio = legVm.Ratio;
                leg.IsPreferred = legVm.IsPreferred;
                portfolioItem.Legs.Add(leg);
            }

            if(!string.IsNullOrEmpty(_endTimePointsExpr))
            {
                string[] tps = _endTimePointsExpr.Split(',');
                foreach(string s in tps)
                {
                    if(!string.IsNullOrWhiteSpace(s))
                    portfolioItem.EndTimePoints.Add(s.Trim());
                }
            }

            portfolioItem.Strategy = StrategySetting.GetEntity();
            
            return portfolioItem;
        }

        public void Update(PTEntity.PortfolioUpdateItem item)
        {
            if (item.StrategyUpdate.Kind == PTEntity.StrategyType.SCALPER)
            {
                PTEntity.ScalperStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.ScalperStrategyUpdateItem;
                if (strategyUpdate != null)
                {
                    Diff = ToDecimal(strategyUpdate.Diff);
                }
            }
            else if(item.StrategyUpdate.Kind == PTEntity.StrategyType.ARBITRAGE ||
                item.StrategyUpdate.Kind == PTEntity.StrategyType.CHANGE_POSITION)
            {
                PTEntity.ArbitrageStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.ArbitrageStrategyUpdateItem;
                LongDiff = ToDecimal(strategyUpdate.LongDiff);
                ShortDiff = ToDecimal(strategyUpdate.ShortDiff);
                LongSize = strategyUpdate.LongSize;
                ShortSize = strategyUpdate.ShortSize;
                AR_BollTop = strategyUpdate.BollTop;
                AR_BollBottom = strategyUpdate.BollBottom;
            }
            else if (item.StrategyUpdate.Kind == PTEntity.StrategyType.HIST_SLOPE)
            {
                PTEntity.HistSlopeStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.HistSlopeStrategyUpdateItem;
                FastAngle = ToDecimal(strategyUpdate.FastAngle);
                SlowAngle = ToDecimal(strategyUpdate.SlowAngle);
                FastMacdHist = ToDecimal(strategyUpdate.FastMacdHist);
                SlowMacdHist = ToDecimal(strategyUpdate.SlowMacdHist);
                FastMacdHistDiff = ToDecimal(strategyUpdate.FastMacdHistDiff);
                SlowMacdHistDiff = ToDecimal(strategyUpdate.SlowMacdHistDiff);
                FastSlopeDirection = strategyUpdate.FastMacdSlopeDirection;
                SlowSlopeDirection = strategyUpdate.SlowMacdSlopeDirection;
            }
            else if (item.StrategyUpdate.Kind == PTEntity.StrategyType.WMA_TREND)
            {
                PTEntity.WMATrendStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.WMATrendStrategyUpdateItem;
                FastLine = ToDecimal(strategyUpdate.FastLine);
                SlowLine = ToDecimal(strategyUpdate.SlowLine);
            }
            else if (item.StrategyUpdate.Kind == PTEntity.StrategyType.LINER_REGRESSION)
            {
                PTEntity.LinerRegStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.LinerRegStrategyUpdateItem;
                WeightMidPoint = ToDecimal(strategyUpdate.WeightMidPoint);
                LinerRegressionAngle = ToDecimal(strategyUpdate.LinerRegAngle);
            }
            else if (item.StrategyUpdate.Kind == PTEntity.StrategyType.ASC_TREND)
            {
                PTEntity.ASCTrendStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.ASCTrendStrategyUpdateItem;
                
                AS_WilliamsR = ToDecimal(strategyUpdate.WilliamsR);
                AS_StopPx = ToDecimal(strategyUpdate.StopPx);
                AS_DonchianHi = ToDecimal(strategyUpdate.DonchianHi);
                AS_DonchianLo = ToDecimal(strategyUpdate.DonchianLo);
            }
            else if (item.StrategyUpdate.Kind == PTEntity.StrategyType.RANGE_TREND)
            {
                PTEntity.RangeTrendStrategyUpdateItem strategyUpdate = item.StrategyUpdate as PTEntity.RangeTrendStrategyUpdateItem;

                RT_UpperBoundOpen = ToDecimal(strategyUpdate.UpperBoundOpen);
                RT_LowerBoundOpen = ToDecimal(strategyUpdate.LowerBoundOpen);
                RT_UpperBoundClose = ToDecimal(strategyUpdate.UpperBoundClose);
                RT_LowerBoundClose = ToDecimal(strategyUpdate.LowerBoundClose);
                RT_LastCostPx = ToDecimal(strategyUpdate.LastCostPx);
                RT_RecentStopLossPx = ToDecimal(strategyUpdate.RecentStopLossPx);
            }

            OpenTimes = item.TotalOpenTimes;
            DoneTimes = item.TotalCloseTimes;
            CancelVolume = item.CancelTimes;
            //Position = item.CurrentPosition;

            Gain = item.Profit;
            //Quantity = item.Quantity;
            //MaxPosition = item.MaxPosition;
            //AvgCost = item.AvgCost;

            IsRunning = item.StrategyUpdate.Running;

            for (int i = 0; i < item.Legs.Count; ++i )
            {
                _legs[i].Update(item.Legs[i]);
            }

            if (item.HasMsg)
            {
                EventLogger.Write(item.Message);
            }
        }

        private static decimal ToDecimal(double val)
        {
            return val >= (double)decimal.MaxValue || val <= (double)decimal.MinValue ? 
                0 : (decimal)Math.Round(val, 2);
        }

        private void OnOpenPosition()
        {
            if (_accountVm.VerifyStatus())
            {
                if (StrategySetting.Name == StrategySetting.ChangePositionStrategyName)
                {
                    _accountVm.Host.SwitchPosition(Id, Quantity);
                }
                else if (StrategySetting.Name == StrategySetting.ScalperStrategyName)
                {
                    _accountVm.Host.ScalperOpenPosition(Id, Quantity);
                }
                else
                {
                    _accountVm.Host.PortfOpenPosition(Id, Quantity, false);
                }

                EventLogger.Write("{0} 开仓组合 {1}, 数量 {2}", _accountVm.InvestorId, DisplayText, Quantity);
            }
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
                bool isVirtual = viewModel.IsVirtual;
                if (_accountVm.VerifyStatus())
                {
                    _accountVm.Host.PortfOpenPosition(Id, qty, isVirtual);
                    EventLogger.Write("{0} 开仓组合 {1}, 数量 {2} - ({3})", _accountVm.InvestorId, DisplayText, qty,
                        isVirtual ? "虚拟" : "真实");
                }
            }
        }

        private void OnClosePosition()
        {
            if (_accountVm.VerifyStatus())
            {
                _accountVm.Host.PortfClosePosition(Id, "", "");
                EventLogger.Write("{0} 平仓组合 {1}", _accountVm.InvestorId, DisplayText);
            }
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
                bool isVirtual = viewModel.IsVirtual;

                if (_accountVm.VerifyStatus())
                {
                    _accountVm.Host.PortfClosePosition(Id, "", "");
                    EventLogger.Write("{0} 平仓组合 {1}, 数量 {2} - ({3})", _accountVm.InvestorId, DisplayText, qty,
                        isVirtual ? "虚拟" : "真实");
                }
            }
        }

        private void OnModifyQuantity()
        {
            var viewModel = new ModifyQtyViewModel();
            viewModel.PortfolioId = Id;
            viewModel.OnceQuantity = Quantity;
            viewModel.TotalOpenLimit = TotalOpenLimit;
            viewModel.MaxOpenPerStart = MaxOpenPerStart;
            viewModel.MaxCancel = MaxCancel;
            viewModel.EndTimePointsExpr = EndTimePointsExpr;
            ModifyMaxQtyDlg dlg = new ModifyMaxQtyDlg(viewModel);
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            bool? ret = dlg.ShowDialog();
            if (ret ?? false)
            {
                Quantity = viewModel.OnceQuantity;
                TotalOpenLimit = viewModel.TotalOpenLimit;
                MaxOpenPerStart = viewModel.MaxOpenPerStart;
                MaxCancel = viewModel.MaxCancel;
                EndTimePointsExpr = viewModel.EndTimePointsExpr;

                if (_accountVm.VerifyStatus())
                {
                    _accountVm.Host.PortfModifyQuantity(Id, Quantity, MaxOpenPerStart, TotalOpenLimit, MaxCancel, viewModel.getEndTimePoints());
                    EventLogger.Write("{0} 修改组合 {1}数量: 每次->{2}, 每组->{3}， 最多->{4}, 撤单->{5}",
                        _accountVm.InvestorId, DisplayText, Quantity, MaxOpenPerStart, TotalOpenLimit, MaxCancel);
                }
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
                int existingOrderCount = 0;
                if (IsRunning)
                {
                    PortfolioOrdersView portfOrdersView = ServiceLocator.Current.GetInstance<PortfolioOrdersView>();
                    if (portfOrdersView != null)
                        existingOrderCount = portfOrdersView.OrderRepositry.GetAccountOrderCount(AccountId, Id);
                }
                
                _accountVm.Host.PortfEnableStrategy(Id, IsRunning, existingOrderCount);
            }
        }

        public void StartStrategy(bool running)
        {
            if (running)
                OnStart();
            else
                OnStop();
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

        public void ApplyStrategySettings(StrategySetting settings)
        {
            this.StrategySetting.CopyFrom(settings);
            _accountVm.Host.PortfApplyStrategySettings(this.Id, StrategySetting.GetEntity());

            _accountVm.PublishChanged();
        }

    }
}

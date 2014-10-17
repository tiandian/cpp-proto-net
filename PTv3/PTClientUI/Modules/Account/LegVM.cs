using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Xml.Linq;
using PTEntity;

namespace PortfolioTrading.Modules.Account
{
    public class LegVM : NotificationObject
    {
        #region Symbol
        private string _symbol;

        public string Symbol
        {
            get { return _symbol; }
            set
            {
                if (_symbol != value)
                {
                    _symbol = value;
                    RaisePropertyChanged("Symbol");
                    RaisePropertyChanged("Name");
                }
            }
        }
        #endregion

        public string Name
        {
            get { return _symbol; }
        }

        #region Ratio
        private int _ratio;

        public int Ratio
        {
            get { return _ratio; }
            set
            {
                if (_ratio != value)
                {
                    _ratio = value;
                    RaisePropertyChanged("Ratio");
                }
            }
        }
        #endregion

        #region Side
        private PosiDirectionType _side;

        public PosiDirectionType Side
        {
            get { return _side; }
            set
            {
                if (_side != value)
                {
                    _side = value;
                    RaisePropertyChanged("Side");
                }
            }
        }
        #endregion

        #region Status
        private LegStatus _status;

        public LegStatus Status
        {
            get { return _status; }
            set
            {
                if (_status != value)
                {
                    _status = value;
                    RaisePropertyChanged("Status");
                }
            }
        }
        #endregion

        #region Last
        private double _last;

        public double Last
        {
            get { return _last; }
            set
            {
                if (_last != value)
                {
                    _last = value;
                    RaisePropertyChanged("Last");
                }
            }
        }
        #endregion

        #region Ask
        private double _ask;

        public double Ask
        {
            get { return _ask; }
            set
            {
                if (_ask != value)
                {
                    _ask = value;
                    RaisePropertyChanged("Ask");
                }
            }
        }
        #endregion

        #region AskSize
        private int askSize;

        public int AskSize
        {
            get { return askSize; }
            set
            {
                if (askSize != value)
                {
                    askSize = value;
                    RaisePropertyChanged("AskSize");
                }
            }
        }
        #endregion

        #region Bid
        private double _bid;

        public double Bid
        {
            get { return _bid; }
            set
            {
                if (_bid != value)
                {
                    _bid = value;
                    RaisePropertyChanged("Bid");
                }
            }
        }
        #endregion

        #region BidSize
        private int _bidSize;

        public int BidSize
        {
            get { return _bidSize; }
            set
            {
                if (_bidSize != value)
                {
                    _bidSize = value;
                    RaisePropertyChanged("BidSize");
                }
            }
        }
        #endregion

        internal void SetIsPreferred(bool val)
        {
            _isPreferred = val;
        }

        #region IsPreferred
        private bool _isPreferred = false;

        public bool IsPreferred
        {
            get { return _isPreferred; }
            set
            {
                if (_isPreferred != value)
                {
                    _isPreferred = value;
                    RaisePropertyChanged("IsPreferred");

                    if(_isPreferred)
                        RaiseIsPreferredChanged(_isPreferred);
                }
            }
        }
        #endregion

        #region MinPriceChange
        private double _minPriceChange;

        public double MinPriceChange
        {
            get { return _minPriceChange; }
            set
            {
                if (_minPriceChange != value)
                {
                    _minPriceChange = value;
                    RaisePropertyChanged("MinPriceChange");
                }
            }
        }
        #endregion
        

        public string PortfolioId { get; set; }

        public event Action<LegVM, bool> OnIsPreferredChanged;

        private void RaiseIsPreferredChanged(bool newVal)
        {
            if (OnIsPreferredChanged != null)
                OnIsPreferredChanged(this, newVal);
        }

        public static LegVM Load(XElement xmlElement)
        {
            LegVM leg = new LegVM();

            XAttribute attr = xmlElement.Attribute("symbol");
            if (attr != null)
                leg.Symbol = attr.Value;

            attr = xmlElement.Attribute("ratio");
            if (attr != null)
            {
                leg.Ratio = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("side");
            if (attr != null)
            {
                leg.Side = (PosiDirectionType)Enum.Parse(typeof(PosiDirectionType), attr.Value);
            }

            attr = xmlElement.Attribute("isPreferred");
            if (attr != null)
            {
                leg.IsPreferred = bool.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("minPriceChange");
            if(attr != null)
            {
                leg.MinPriceChange = double.Parse(attr.Value);
            }

            return leg;
        }

        public XElement Persist()
        {
            XElement elem = new XElement("leg");
            elem.Add(new XAttribute("symbol", _symbol));
            elem.Add(new XAttribute("ratio", _ratio));
            elem.Add(new XAttribute("side", _side.ToString()));
            elem.Add(new XAttribute("isPreferred", _isPreferred.ToString()));
            elem.Add(new XAttribute("minPriceChange", _minPriceChange.ToString()));
            return elem;
        }

        public void Update(PTEntity.LegUpdateItem item)
        {
            //Status = item.Status;
            Last = item.Last;
            Ask = item.Ask == double.MaxValue ? 0 : item.Ask;
            Bid = item.Bid == double.MaxValue ? 0 : item.Bid;
            AskSize = item.AskSize == int.MaxValue ? 0 : item.AskSize;
            BidSize = item.BidSize == int.MaxValue ? 0 : item.BidSize;
        }
    }

}

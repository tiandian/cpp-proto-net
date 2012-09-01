using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Account
{
    public class PortfolioVM : NotificationObject
    {
        private ObservableCollection<LegVM> _legs = new ObservableCollection<LegVM>();

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
        private double _diff;

        public double Diff
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
                }
            }
        }
        #endregion

        #region AutoClose
        private bool _autoClose;

        public bool AutoClose
        {
            get { return _autoClose; }
            set
            {
                if (_autoClose != value)
                {
                    _autoClose = value;
                    RaisePropertyChanged("AutoClose");
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


        public IEnumerable<LegVM> Legs
        {
            get { return _legs; }
        }

        public void AddLeg(LegVM leg)
        {
            _legs.Add(leg);
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

        public static PortfolioVM Load(XElement xmlElement)
        {
            PortfolioVM portf = new PortfolioVM();

            XAttribute attr = xmlElement.Attribute("id");
            if (attr != null)
                portf.Id = attr.Value;

            attr = xmlElement.Attribute("quantity");
            if (attr != null)
            {
                portf.Quantity = int.Parse(attr.Value);
            }

            attr = xmlElement.Attribute("autoOpen");
            if (attr != null)
            {
                portf.AutoOpen = attr.Value == bool.TrueString;
            }

            attr = xmlElement.Attribute("autoClose");
            if (attr != null)
            {
                portf.AutoClose = attr.Value == bool.TrueString;
            }

            foreach (var legElem in xmlElement.Element("legs").Elements("leg"))
            {
                LegVM legVm = LegVM.Load(legElem);
                portf._legs.Add(legVm);
            }
            return portf;
        }

        public XElement Persist()
        {
            XElement elem = new XElement("portfolio");
            elem.Add(new XAttribute("id", _id));
            elem.Add(new XAttribute("quantity", _qty));
            elem.Add(new XAttribute("autoOpen", _autoOpen.ToString()));
            elem.Add(new XAttribute("autoClose", _autoClose.ToString()));

            XElement elemLegs = new XElement("legs");
            foreach (var l in _legs)
            {
                elemLegs.Add(l.Persist());
            }
            elem.Add(elemLegs);

            return elem;
        }

        public entity.PortfolioItem GetEntity()
        {
            entity.PortfolioItem portfolioItem = new entity.PortfolioItem();
            portfolioItem.ID = Id;
            portfolioItem.AutoOpen = AutoOpen;
            portfolioItem.Diff = Diff;
            portfolioItem.Quantity = Quantity;

            foreach (var legVm in _legs)
            {
                entity.LegItem leg = new entity.LegItem();
                leg.Symbol = legVm.Symbol;
                leg.Side = legVm.Side;
                leg.Ratio = legVm.Ratio;
                portfolioItem.Legs.Add(leg);
            }

            return portfolioItem;
        }

        public void Update(entity.PortfolioItem item)
        {
            Diff = item.Diff;

            for (int i = 0; i < item.Legs.Count; ++i )
            {
                _legs[i].Update(item.Legs[i]);
            }
        }
    }
}

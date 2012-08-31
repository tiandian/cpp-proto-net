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

        public IEnumerable<LegVM> Legs
        {
            get { return _legs; }
        }

        public void AddLeg(LegVM leg)
        {
            _legs.Add(leg);
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
    }
}

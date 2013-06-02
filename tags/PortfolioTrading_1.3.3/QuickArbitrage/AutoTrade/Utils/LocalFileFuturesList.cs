using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using System.Xml.Linq;
using QuickArbitrage.Common;

namespace AutoTrade.MainFrame.Utils
{
    [Export(typeof(IFuturesRepositry))]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class LocalFileFuturesList : IFuturesRepositry
    {
        private List<Future> _futuresList;
        private List<ExchangeInfo> _exchanges;

        public LocalFileFuturesList()
        {
            _futuresList = new List<Future>();
            _exchanges = new List<ExchangeInfo>();

            Load("Data\\AllFutrues.xml");
        }

        private void Load(string path)
        {
            try
            {
                XElement futuresDoc = XElement.Load(path);
                var exchanges = futuresDoc.Elements("exchange");
                foreach (var exElem in exchanges)
                {
                    string exId = exElem.Attribute("Id").Value;
                    ExchangeID exch = (ExchangeID)Enum.Parse(typeof(ExchangeID), exId);
                    string exName = exElem.Attribute("Name").Value;

                    ExchangeInfo exInfo = new ExchangeInfo();
                    exInfo.ID = exch;
                    exInfo.Name = exName;
                    _exchanges.Add(exInfo);

                    var fuElems = exElem.Elements("future");
                    foreach (var futureElem in fuElems)
                    {
                        string fID = futureElem.Attribute("Id").Value;
                        string fName = futureElem.Attribute("Name").Value;

                        Future f = new Future();
                        f.Id = fID;
                        f.Name = fName;
                        f.ExchangeID = exch;
                        f.ExchName = exName;

                        _futuresList.Add(f);
                    }
                }
            }
            catch (System.Exception ex)
            {
                LogManager.Logger.Error(ex.Message);
            }
        }

        public IEnumerable<QuickArbitrage.Common.Future> AllFutures
        {
            get { return _futuresList; }
        }

        public IEnumerable<QuickArbitrage.Common.ExchangeInfo> AllExchanges
        {
            get { return _exchanges; }
        }

        public IEnumerable<QuickArbitrage.Common.Future> GetFuturesByExchange(QuickArbitrage.Common.ExchangeID exId)
        {
            return from f in _futuresList where f.ExchangeID == exId select f;
        }
    }
}

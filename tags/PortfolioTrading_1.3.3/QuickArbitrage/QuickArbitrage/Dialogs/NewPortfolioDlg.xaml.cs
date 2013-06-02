using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using QuickArbitrage.Main.Util;
using QuickArbitrage.Common.Enums;
using System.Collections.ObjectModel;
using QuickArbitrage.Common;
using QuickArbitrage.Main.ViewModel;
using Infragistics.Controls.Grids;
using System.Collections.Specialized;
using System.ComponentModel;

namespace QuickArbitrage.Main.Dialogs
{
    /// <summary>
    /// Interaction logic for NewPortfolioDlg.xaml
    /// </summary>
    public partial class NewPortfolioDlg : Window
    {
        private ComposedFutureList _composition;

        public NewPortfolioDlg()
        {
            InitializeComponent();

            this.availExchList.ItemsSource = FuturesRepositry.Instance.AllExchanges;

            _composition = (ComposedFutureList)selectedItemsPanel.FindResource("futuresList");
            
        }

        public bool IsOK { get; private set; }

        public RunningPortfolioItem NewPortfolio
        {
            get
            {
                RunningPortfolioItem portfolio = new RunningPortfolioItem();
               
                foreach (var f in _composition)
                {
                    EquityItem eq = new EquityItem();

                    portfolio.Equities.Add(eq);
                }
                return portfolio;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (this.availExchList.Items.Count > 0)
                this.availExchList.SelectedIndex = 0;

            _composition.Clear();
        }

        private void GetFuturesByExchange(ExchangeID exID)
        {
            
            List<FutureContractItem> futureItems = new List<FutureContractItem>();
            var futures = FuturesRepositry.Instance.GetFuturesByExchange(exID);

            Random rand = new Random();
            foreach (var f in futures)
            {
                futureItems.Add(new FutureContractItem()
                                {
                                     FutureCode = f.Id,
                                     FutureName = f.Name,
                                     Last = rand.Next(1000, 2000)
                                });
            }

            xamGridAvailContracts.ItemsSource = futureItems;
        }

        private void availExchList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ExchangeInfo exInfo = availExchList.SelectedItem as ExchangeInfo;
            if(exInfo != null)
            {
                this.Dispatcher.BeginInvoke(new Action<ExchangeID>(GetFuturesByExchange),
                    System.Windows.Threading.DispatcherPriority.Loaded, exInfo.ID);
            }
            
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            this.IsOK = true;
            this.Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.IsOK = false;
            this.Close();
        }

        private void xamGridAvailContracts_SelectedRowsCollectionChanged(object sender, Infragistics.Controls.Grids.SelectionCollectionChangedEventArgs<Infragistics.Controls.Grids.SelectedRowsCollection> e)
        {
            if (e.NewSelectedItems.Count > 0)
            {
                Row row = e.NewSelectedItems[0];
                FutureContractItem futureItem = row.Data as FutureContractItem;

                if (_composition.Count > 1)
                {
                    _composition.RemoveAt(1);
                }

                ComposedFutureViewModel fvm = new ComposedFutureViewModel();
                fvm.ID = futureItem.FutureCode;
                fvm.Name = futureItem.FutureName;
                fvm.Last = futureItem.Last;
                fvm.Action = _composition.Count == 0 ? TradeAction.LongEntry : TradeAction.ShortEntry;

                _composition.Add(fvm);
            }
        }

        private void Button_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Button btn = sender as Button;
            if (btn != null)
            {
                ComposedFutureViewModel fvm = btn.Tag as ComposedFutureViewModel;
                if (fvm != null)
                {
                    _composition.Remove(fvm);
                }
            }
        }
    }

    public class TradeActionVM
    {
        public TradeAction TradeAction { get; set; }

        public string Name { get; set; }

        public TradeActionVM(TradeAction ta, string taName)
        {
            this.TradeAction = ta;
            this.Name = taName;
        }
    }

    public class TradeActionList : List<TradeActionVM>
    {
        public TradeActionList()
        {
            this.Add(new TradeActionVM(TradeAction.LongEntry, "多头开仓"));
            this.Add(new TradeActionVM(TradeAction.ShortEntry, "空头开仓"));
        }
    }

    public class ComposedFutureViewModel : BaseViewModel
    {
        #region ID
        
        private string _id = string.Empty;
        public string ID
        {
        	get { return _id; }
        	set
        	{
        		if (_id != value)
        		{
        			_id = value;
        			this.OnPropertyChanged("ID");
        		}
        	}
        }
        #endregion

        #region Name
        
        private string _name = string.Empty;
        public string Name
        {
        	get { return _name; }
        	set
        	{
        		if (_name != value)
        		{
        			_name = value;
        			this.OnPropertyChanged("Name");
        		}
        	}
        }
        #endregion

        #region Last
        
        private float _last;
        public float Last
        {
        	get { return _last; }
        	set
        	{
        		if (_last != value)
        		{
        			_last = value;
        			this.OnPropertyChanged("Last");
        		}
        	}
        }
        #endregion

        #region Action
        
        private TradeAction _action = TradeAction.LongEntry;
        public TradeAction Action
        {
        	get { return _action; }
        	set
        	{
        		if (_action != value)
        		{
        			_action = value;
        			this.OnPropertyChanged("Action");
        		}
        	}
        }
        #endregion

        #region Ratio
        
        private float _ratio = 1.0f;
        public float Ratio
        {
        	get { return _ratio; }
        	set
        	{
        		if (_ratio != value)
        		{
        			_ratio = value;
        			this.OnPropertyChanged("Ratio");
        		}
        	}
        }
        #endregion
    }

    public class ComposedFutureList : ObservableCollection<ComposedFutureViewModel>
    {
        public ComposedFutureList()
        {
            this.CollectionChanged += new NotifyCollectionChangedEventHandler(ComposedFutureList_CollectionChanged);

            this.Add(new ComposedFutureViewModel() { Action = TradeAction.LongEntry });
            this.Add(new ComposedFutureViewModel() { Action = TradeAction.ShortEntry });
        }

        void ComposedFutureList_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == NotifyCollectionChangedAction.Add)
            {
                // add event handler
                foreach (var item in e.NewItems)
                {
                    INotifyPropertyChanged notifyItem = item as INotifyPropertyChanged;
                    if (notifyItem != null)
                    {
                        notifyItem.PropertyChanged += notifyItem_PropertyChanged;
                    }
                }
            }
            else if (e.Action == NotifyCollectionChangedAction.Remove || e.Action == NotifyCollectionChangedAction.Reset)
            {
                // remove event handler
                if (e.NewItems != null)
                {
                    foreach (var item in e.NewItems)
                    {
                        INotifyPropertyChanged notifyItem = item as INotifyPropertyChanged;
                        if (notifyItem != null)
                        {
                            notifyItem.PropertyChanged -= notifyItem_PropertyChanged;
                        }
                    }
                }
            }

            UpdateDiff();
            UpdateHedge();
        }

        void notifyItem_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Ratio" || e.PropertyName == "Action")
            {
                UpdateHedge();
            }
            
        }



        private void UpdateDiff()
        {
            if (this.Count >= 2)
            {
                PriceDiff = Math.Abs(this[0].Last - this[1].Last);
            }
            else
            {
                PriceDiff = float.NaN;
            }
        }

        private void UpdateHedge()
        {
            if (this.Count >= 2)
            {
                float larger = Math.Max(this[0].Ratio, this[1].Ratio);
                float diff = Math.Abs(this[0].Ratio - this[1].Ratio);
                float hedge = this[0].Action != this[1].Action ? 1 - diff / larger : 0;
                Hedge = (float)Math.Round(hedge, 2);
            }
            else
            {
                Hedge = float.NaN;
            }
        }

        private float _priceDiff = 0.0f;
        public float PriceDiff
        {
            set 
            {
                _priceDiff = value;
                this.OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("PriceDiff"));
            }
            get 
            {
                return _priceDiff;
            }
        }

        private float _hedge = 0.0f;
        public float Hedge
        {
            set
            {
                _hedge = value;
                this.OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Hedge"));
            }
            get
            {
                return _hedge;
            }
        }
    }
}

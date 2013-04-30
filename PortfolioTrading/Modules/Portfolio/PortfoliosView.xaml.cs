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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Modules.Account;
using Infragistics.Windows.DataPresenter;

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for PortfoliosView.xaml
    /// </summary>
    [Export]
    [PartCreationPolicy(CreationPolicy.NonShared)]
    public partial class PortfoliosView : UserControl
    {
        private IEventAggregator EventAgg { get; set; }

        [ImportingConstructor]
        public PortfoliosView(IEventAggregator evtAgg)
        {
            EventAgg = evtAgg;
            InitializeComponent();
        }

        public void SetAccount(AccountVM accountVm)
        {
            this.DataContext = accountVm;
        }

        public void OnViewSelected()
        {
            if (this.xamDG_Portfolio.ActiveRecord != null)
            {
                if (this.xamDG_Portfolio.ActiveRecord.IsDataRecord)
                {
                    PublishPortfolioSelected(this.xamDG_Portfolio.ActiveRecord as DataRecord);
                }
            }
        }

        private void xamDG_Portfolio_RecordActivated(object sender, Infragistics.Windows.DataPresenter.Events.RecordActivatedEventArgs e)
        {
            if (e.Record.IsDataRecord)
            {
                PublishPortfolioSelected(e.Record as DataRecord);
            }
        }

        private void PublishPortfolioSelected(DataRecord dr)
        {
            if (dr != null)
            {
                PortfolioVM portf = dr.DataItem as PortfolioVM;
                EventAgg.GetEvent<PortfolioSelectedEvent>().Publish(portf);
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if(this.xamDG_Portfolio.ActiveRecord == null)
            {
                if (this.xamDG_Portfolio.Records.Count > 0)
                    this.xamDG_Portfolio.Records[0].IsActive = true;
            }
        }
    }
}

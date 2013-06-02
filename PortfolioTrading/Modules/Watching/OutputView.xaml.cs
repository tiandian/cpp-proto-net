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

namespace PortfolioTrading.Modules.Watching
{
    /// <summary>
    /// Interaction logic for Output.xaml
    /// </summary>
    [Export]
    public partial class OutputView : UserControl
    {
        [ImportingConstructor]
        public OutputView(IEventAggregator evtAgg)
        {
            InitializeComponent();
            evtAgg.GetEvent<OutputEvent>().Subscribe(PrintMessage, ThreadOption.UIThread);
        } 

        private void PrintMessage(string message)
        {
            string txt = string.Format("{0} {1}\n", DateTime.Now.ToLongTimeString(), message);
            this.txtOutput.AppendText(txt);
        }
    }
}

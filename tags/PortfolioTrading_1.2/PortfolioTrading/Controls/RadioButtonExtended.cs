using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;

namespace PortfolioTrading.Controls
{
    public class RadioButtonExtended : RadioButton
    {
        static bool m_bIsChanging = false;

        public RadioButtonExtended()
        {
            this.Checked += new RoutedEventHandler(RadioButtonExtended_Checked);
            this.Unchecked += new RoutedEventHandler(RadioButtonExtended_Unchecked);
        }

        void RadioButtonExtended_Unchecked(object sender, RoutedEventArgs e)
        {
            if (!m_bIsChanging)
                this.IsCheckedReal = false;
        }

        void RadioButtonExtended_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_bIsChanging)
                this.IsCheckedReal = true;
        }

        public bool? IsCheckedReal
        {
            get { return (bool?)GetValue(IsCheckedRealProperty); }
            set
            {
                SetValue(IsCheckedRealProperty, value);
            }
        }

        // Using a DependencyProperty as the backing store for IsCheckedReal. This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsCheckedRealProperty =
        DependencyProperty.Register("IsCheckedReal", typeof(bool?), typeof(RadioButtonExtended),
        new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.Journal |
        FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
        IsCheckedRealChanged));

        public static void IsCheckedRealChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            m_bIsChanging = true;
            ((RadioButtonExtended)d).IsChecked = (bool)e.NewValue;
            m_bIsChanging = false;
        }
    }

}

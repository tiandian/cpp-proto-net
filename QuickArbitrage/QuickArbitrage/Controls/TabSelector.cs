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
using System.Collections;

namespace QuickArbitrage.Main.Controls
{
    /// <summary>
    /// Follow steps 1a or 1b and then 2 to use this custom control in a XAML file.
    ///
    /// Step 1a) Using this custom control in a XAML file that exists in the current project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:QuickArbitrage.Main.Controls"
    ///
    ///
    /// Step 1b) Using this custom control in a XAML file that exists in a different project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:QuickArbitrage.Main.Controls;assembly=QuickArbitrage.Main.Controls"
    ///
    /// You will also need to add a project reference from the project where the XAML file lives
    /// to this project and Rebuild to avoid compilation errors:
    ///
    ///     Right click on the target project in the Solution Explorer and
    ///     "Add Reference"->"Projects"->[Browse to and select this project]
    ///
    ///
    /// Step 2)
    /// Go ahead and use your control in the XAML file.
    ///
    ///     <MyNamespace:TabSelector/>
    ///
    /// </summary>
    public class TabSelector : Control
    {
        static TabSelector()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(TabSelector), new FrameworkPropertyMetadata(typeof(TabSelector)));
        }

        public event EventHandler<TabSelectionChangedEventArgs> TabSelectionChanged;
        private ListBox _lbTabs = null;

        public override void OnApplyTemplate()
        {
            _lbTabs = this.Template.FindName("lbTabs", this) as ListBox;

            if (_lbTabs.Items.Count > 0 && SelectedIndex > -1)
            {
                _lbTabs.SelectedIndex = SelectedIndex;
            }

            _lbTabs.SelectionChanged += new SelectionChangedEventHandler(_lbTabs_SelectionChanged);
            
            base.OnApplyTemplate();
        }

        void _lbTabs_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (TabSelectionChanged != null)
            {
                if (e.AddedItems != null && e.AddedItems.Count > 0)
                    TabSelectionChanged(this, new TabSelectionChangedEventArgs(e.AddedItems[0]));
            }
        }

        public IEnumerable DataSource
        {
            get { return (IEnumerable)GetValue(DataSourceProperty); }
            set { SetValue(DataSourceProperty, value); }
        }

        // Using a DependencyProperty as the backing store for DataSource.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty DataSourceProperty =
            DependencyProperty.Register("DataSource", typeof(IEnumerable), typeof(TabSelector), new UIPropertyMetadata(null));


        public DataTemplate ItemTemplate
        {
            get { return (DataTemplate)GetValue(ItemTemplateProperty); }
            set { SetValue(ItemTemplateProperty, value); }
        }

        // Using a DependencyProperty as the backing store for ItemTemplate.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ItemTemplateProperty =
            DependencyProperty.Register("ItemTemplate", typeof(DataTemplate), typeof(TabSelector), new UIPropertyMetadata(null));



        public int SelectedIndex
        {
            get { return (int)GetValue(SelectedIndexProperty); }
            set { SetValue(SelectedIndexProperty, value); }
        }

        // Using a DependencyProperty as the backing store for SelectedIndex.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SelectedIndexProperty =
            DependencyProperty.Register("SelectedIndex", typeof(int), typeof(TabSelector),
                                        new UIPropertyMetadata(-1));
        //                                                        new PropertyChangedCallback(OnSelectedIndexChanged)));

        //static void OnSelectedIndexChanged(DependencyObject dpObj, DependencyPropertyChangedEventArgs args)
        //{
        //    TabSelector tabs = dpObj as TabSelector;
        //    if (tabs._lbTabs != null)
        //    {
        //        int val = (int)args.NewValue;
        //        if (val < tabs._lbTabs.Items.Count)
        //        {
        //            tabs._lbTabs.SelectedIndex = val;
        //        }
        //    }
        //}

        public object SelectedItem
        {
            get { return (object)GetValue(SelectedItemProperty); }
            set { SetValue(SelectedItemProperty, value); }
        }

        // Using a DependencyProperty as the backing store for SelectedItem.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SelectedItemProperty =
            DependencyProperty.Register("SelectedItem", typeof(object), typeof(TabSelector), new UIPropertyMetadata(null));


    }

    public class TabSelectionChangedEventArgs : EventArgs
    {
        public object SelectedItem { get; private set; }

        public TabSelectionChangedEventArgs(object selectedItem)
        {
            this.SelectedItem = selectedItem;
        }
    }
}

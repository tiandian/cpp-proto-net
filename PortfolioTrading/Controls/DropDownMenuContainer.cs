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
using System.Windows.Markup;
using System.Windows.Controls.Primitives;
using System.ComponentModel;

namespace PortfolioTrading.Controls
{
    /// <summary>
    /// Follow steps 1a or 1b and then 2 to use this custom control in a XAML file.
    ///
    /// Step 1a) Using this custom control in a XAML file that exists in the current project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:PortfolioTrading.Controls"
    ///
    ///
    /// Step 1b) Using this custom control in a XAML file that exists in a different project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:PortfolioTrading.Controls;assembly=PortfolioTrading.Controls"
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
    ///     <MyNamespace:DropDownMenuContainer/>
    ///
    /// </summary>
    [ContentProperty("Items")]
    [DefaultProperty("Items")]
    public class DropDownMenuContainer : Control
    {
        static DropDownMenuContainer()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(DropDownMenuContainer), new FrameworkPropertyMetadata(typeof(DropDownMenuContainer)));
        }

        /// <summary>
        /// The Split Button's Items property maps to the base classes ContextMenu.Items property
        /// </summary>
        public ItemCollection Items
        {
            get
            {
                EnsureContextMenuIsValid();
                return this.ContextMenu.Items;
            }
        }

        /// <summary>
        /// Make sure the Context menu is not null
        /// </summary>
        private void EnsureContextMenuIsValid()
        {
            if (this.ContextMenu == null)
            {
                this.ContextMenu = new ContextMenu();
                this.ContextMenu.PlacementTarget = this;
                this.ContextMenu.Placement = PlacementMode.Bottom;

                this.ContextMenu.Opened += ((sender, routedEventArgs) => IsContextMenuOpen = true);
                this.ContextMenu.Closed += ((sender, routedEventArgs) => IsContextMenuOpen = false);
            }
        }

        public string Caption
        {
            get { return (string)GetValue(CaptionProperty); }
            set { SetValue(CaptionProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Caption.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty CaptionProperty =
            DependencyProperty.Register("Caption", typeof(string), typeof(DropDownMenuContainer), new UIPropertyMetadata(""));

        public bool IsContextMenuOpen
        {
            get { return (bool)GetValue(IsContextMenuOpenProperty); }
            set { SetValue(IsContextMenuOpenProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsContextMenuOpen.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsContextMenuOpenProperty =
            DependencyProperty.Register("IsContextMenuOpen", typeof(bool), typeof(DropDownMenuContainer), new UIPropertyMetadata(false));

        Button btnDropDown;
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            btnDropDown = (Button)this.Template.FindName("dropDownButton", this);
            if(btnDropDown != null)
                btnDropDown.Click += new RoutedEventHandler(btnDropDown_Click);
        }

        void btnDropDown_Click(object sender, RoutedEventArgs e)
        {
            OnDropdown();
        }

        private void OnDropdown()
        {
            EnsureContextMenuIsValid();
            if (!this.ContextMenu.HasItems)
                return;

            this.ContextMenu.IsOpen = !IsContextMenuOpen; // open it if closed, close it if open
        }
    }
}

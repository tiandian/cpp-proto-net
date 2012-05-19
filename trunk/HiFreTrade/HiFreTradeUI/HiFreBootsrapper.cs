using System;
using System.Collections.Generic;
using System.ComponentModel.Composition.Hosting;
using System.Linq;
using System.Text;
using System.Windows;
using Microsoft.Practices.Prism.Logging;
using Microsoft.Practices.Prism.MefExtensions;

namespace HiFreTradeUI
{
    class HiFreBootsrapper : MefBootstrapper
    {
        protected override void ConfigureAggregateCatalog()
        {
            this.AggregateCatalog.Catalogs.Add(new AssemblyCatalog(typeof(HiFreBootsrapper).Assembly));
        }

        protected override void ConfigureContainer()
        {
            base.ConfigureContainer();
        }

        protected override void InitializeShell()
        {
            base.InitializeShell();

            Application.Current.MainWindow = (MainWindow)this.Shell;
            
        }

        protected override void InitializeModules()
        {
            base.InitializeModules();

            LoginWnd wnd = this.Container.GetExportedValue<LoginWnd>();
            bool? succ = wnd.ShowDialog();
            if (succ.HasValue && succ.Value)
            {
                Application.Current.MainWindow.Show();
            }
            else
            {
                Logger.Log("Login failed, exit.", Category.Info, Priority.Medium);
                Application.Current.ShutdownMode = ShutdownMode.OnExplicitShutdown;
                Application.Current.Shutdown();
            }
        }

        protected override DependencyObject CreateShell()
        {
            return this.Container.GetExportedValue<MainWindow>();
        }

        protected override ILoggerFacade CreateLogger()
        {
            // Because the Shell is displayed after most of the interesting boostrapper work has been performed,
            // this quickstart uses a special logger class to hold on to early log entries and display them 
            // after the UI is visible.
            return LogManager.Logger as ILoggerFacade;
        }
    }
}

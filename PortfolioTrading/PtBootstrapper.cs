using System;
using System.Collections.Generic;
using System.ComponentModel.Composition.Hosting;
using System.Linq;
using System.Text;
using System.Windows;
using Microsoft.Practices.Prism.MefExtensions;
using Microsoft.Practices.Prism.Logging;

namespace PortfolioTrading
{
    class PtBootstrapper : MefBootstrapper
    {
        protected override void ConfigureAggregateCatalog()
        {
            this.AggregateCatalog.Catalogs.Add(new AssemblyCatalog(typeof(PtBootstrapper).Assembly));
        }

        protected override void ConfigureContainer()
        {
            base.ConfigureContainer();
        }

        protected override void InitializeShell()
        {
            base.InitializeShell();
#if (TEST)
            Application.Current.MainWindow = (MainWindow)this.Shell;
#else
            Application.Current.MainWindow = (Shell)this.Shell;
#endif
        }

        protected override void InitializeModules()
        {
            base.InitializeModules();

            // just show main window
            Application.Current.MainWindow.Show();
            
            /* Don't need login so far
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
             * */
        }

        protected override DependencyObject CreateShell()
        {
#if (TEST)
            return new MainWindow();
#else
            return this.Container.GetExportedValue<Shell>();
#endif
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

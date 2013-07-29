using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace PortfolioTrading
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            LogManager.Logger.Info("Application start up");
#if (DEBUG)
            RunInDebugMode();
#else
            RunInReleaseMode();
#endif
        }

        private void RunInDebugMode()
        {
            PtBootstrapper bootstrapper = new PtBootstrapper();
            bootstrapper.Run();
        }

        private void RunInReleaseMode()
        {
            this.DispatcherUnhandledException += new System.Windows.Threading.DispatcherUnhandledExceptionEventHandler(App_DispatcherUnhandledException);
            AppDomain.CurrentDomain.UnhandledException += AppDomainUnhandledException;
            try
            {
                PtBootstrapper bootstrapper = new PtBootstrapper();
                bootstrapper.Run();
            }
            catch (Exception ex)
            {
                HandleException(ex);
            }
        }

        void App_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            e.Handled = true;
            HandleException(e.Exception);
        }

        private static void AppDomainUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            HandleException(e.ExceptionObject as Exception, e.IsTerminating);
        }

        private static void HandleException(Exception ex, bool isFatal = false)
        {
            if (ex == null)
                return;

            if (isFatal)
                LogManager.Logger.Fatal(ex);
            else
                LogManager.Logger.Error(ex);

            MessageBox.Show(ex.Message, "程序出现异常", MessageBoxButton.OK, MessageBoxImage.Stop);
        }
    }
}

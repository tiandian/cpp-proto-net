using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace PortfolioTrading.Infrastructure
{
    class NativeHost
    {
        public const string HostName = "TradeStation.exe";

        private Process _hostProc;

        public bool Startup(int port, bool showConsole = true)
        {
            _hostProc = new Process();
            _hostProc.StartInfo.FileName = HostName;
            _hostProc.StartInfo.Arguments = "--port " + port.ToString();

            if (!showConsole)
            {
                _hostProc.StartInfo.UseShellExecute = false;
                _hostProc.StartInfo.CreateNoWindow = true;
                _hostProc.StartInfo.RedirectStandardInput = true;
            }
            else
            {
                _hostProc.StartInfo.UseShellExecute = true;
                _hostProc.StartInfo.CreateNoWindow = false;
                _hostProc.StartInfo.RedirectStandardInput = false;
            }

            _hostProc.EnableRaisingEvents = true;
            _hostProc.Exited += host_Exited;

            return _hostProc.Start();
        }

        private void host_Exited(object sender, EventArgs e)
        {
            Debug.WriteLine("NativeServer has exist");
        }

        public void Exit()
        {
            if (_hostProc == null) return;

            // let server process quit
            if (_hostProc.StartInfo.RedirectStandardInput)
                _hostProc.StandardInput.WriteLine("q");
            else
                _hostProc.Kill();
        }
    }
}

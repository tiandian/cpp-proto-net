using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;

namespace AutoTrade.MainFrame.Modules.Portfolio
{
    [ModuleExport(typeof(PortfolioModule))]
    public class PortfolioModule : IModule
    {
        public void Initialize()
        {
            LogManager.Logger.Debug("loading Portfolio module.");
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using System.ComponentModel.Composition;
using Microsoft.Practices.ServiceLocation;

namespace AutoTrade.MainFrame.Modules.Account
{
    [ModuleExport(typeof(AccountModule))]
    public class AccountModule : IModule
    {
        private IRegionManager _regionManager;
        private IServiceLocator _container;

        [ImportingConstructor]
        public AccountModule(IServiceLocator contianer, IRegionManager regionManager)
        {
            _container = contianer;
            _regionManager = regionManager;
        }

        public void Initialize()
        {
            LogManager.Logger.Debug("loading Account module.");
            this._regionManager.RegisterViewWithRegion(Region.RegionNames.AccountTreeRegion,
                () => this._container.GetInstance<AccountTree>());
            this._regionManager.RegisterViewWithRegion(Region.RegionNames.AccountDetailsRegion,
                () => this._container.GetInstance<AccountDetail>());
        }
    }
}

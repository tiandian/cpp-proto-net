using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.ServiceLocation;

namespace PortfolioTrading.Modules.Account
{
    [ModuleExport(typeof(AccountModule))]
    public class AccountModule : IModule
    {
        public void Initialize()
        {
            LogManager.Logger.Info("Account module initializing...");
            IRegionManager regionMgr = ServiceLocator.Current.GetInstance<IRegionManager>();
            regionMgr.RegisterViewWithRegion(RegionNames.AccountMgrRegion,
                () => ServiceLocator.Current.GetInstance<AccountMgrView>());
            regionMgr.RegisterViewWithRegion(RegionNames.AccountInfoRegion,
                () => ServiceLocator.Current.GetInstance<AccountInfoView>());

        }
    }
}

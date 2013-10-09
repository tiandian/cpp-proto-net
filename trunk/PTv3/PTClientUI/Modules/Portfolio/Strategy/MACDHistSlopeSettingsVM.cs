using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using System.Windows.Input;
using Microsoft.Practices.Prism.Commands;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class MACDHistSlopeSettingsVM : StrategySettingVM
    {
        private readonly DelegateCommand _fastStdDiffEditCommand;
        private readonly DelegateCommand _slowStdDiffEditCommand;

        public MACDHistSlopeSettingsVM()
        {
            _fastStdDiffEditCommand = new DelegateCommand(delegate
                {
                    double val;
                    if(CalcStdDiffVal(out val))
                    {
                        MACDHistSlopeStrategySetting setting = (MACDHistSlopeStrategySetting)this.Settings;
                        setting.FastStdDiff = val;
                    }
                });
            _slowStdDiffEditCommand = new DelegateCommand(delegate
                {
                    double val;
                    if (CalcStdDiffVal(out val))
                    {
                        MACDHistSlopeStrategySetting setting = (MACDHistSlopeStrategySetting)this.Settings;
                        setting.SlowStdDiff = val;
                    }

                });
        }

        public ICommand FastStdDiffEditCommand
        {
            get { return _fastStdDiffEditCommand; }
        }

        public ICommand SlowStdDiffEditCommand
        {
            get { return _slowStdDiffEditCommand; }
        }

        protected override StrategySetting CreateSettings()
        {
            return new MACDHistSlopeStrategySetting();
        }

        bool CalcStdDiffVal(out double outVal)
        {
            EditStdDiffDlg dlg = new EditStdDiffDlg();
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            bool? ret = dlg.ShowDialog();
            if (ret ?? false)
            {
                outVal = dlg.StdDiffVal;
                return true;
            }
            else
            {
                outVal = 0.0;
            }
            return false;
        }
    }
}

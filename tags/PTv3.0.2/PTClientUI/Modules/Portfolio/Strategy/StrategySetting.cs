using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ComponentModel;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public abstract class StrategySetting : INotifyPropertyChanged
    {
        public const string ArbitrageStrategyName = "ArbitrageStrategy";
        public const string ChangePositionStrategyName = "ChangePosition";
        public const string ScalperStrategyName = "Scalper";
        public const string TrendStrategyName = "TrendStrategy";

        public abstract string Name { get; }

        public abstract string Persist();

        public abstract void Load(string xmlText);

        public abstract PTEntity.StrategyItem GetEntity();

        public abstract void CopyFrom(StrategySetting settings);

        public static string GetDisplayStrategyName(string strategyName)
        {
            switch (strategyName)
            {
                case ArbitrageStrategyName:
                    return "套利";
                case ChangePositionStrategyName:
                    return "移仓";
                case ScalperStrategyName:
                    return "高频";
                case TrendStrategyName:
                    return "趋势";
            }
            return "未知";
        }

        public static StrategySetting Load(string name, string xmlText)
        {
            StrategySetting setting = Create(name);
            setting.Load(xmlText);
            return setting;
        }

        public static StrategySetting Create(string name)
        {
            StrategySetting setting = null;
            switch (name)
            {
                case ArbitrageStrategyName:
                    setting = new ArbitrageStrategySetting();
                    break;
                case ChangePositionStrategyName:
                    setting = new ChangePositionSetting();
                    break;
                case ScalperStrategyName:
                    setting = new ScalperSetting();
                    break;
                default:
                    throw new ArgumentException(string.Format("Unexpected strategy setting ({0})", name));
            }
            return setting;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged(string propName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }
    }
}

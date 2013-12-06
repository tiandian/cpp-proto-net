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
        public const string MACDHistSlopeStrategyName = "MACDHistSlope";
        public const string WMATrendStrategyName = "WMATrend";
        public const string LinerRegressionStrategyName = "LinerRegression";
        public const string ASCTrendStrategyName = "ASCTrend";

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
                case MACDHistSlopeStrategyName:
                    return "MACD角度";
                case WMATrendStrategyName:
                    return "MACD金叉";
                case LinerRegressionStrategyName:
                    return "线性回归";
                case ASCTrendStrategyName:
                    return "ASCTrend";
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
                case MACDHistSlopeStrategyName:
                    setting = new MACDHistSlopeStrategySetting();
                    break;
                case WMATrendStrategyName:
                    setting = new WMATrendStrategySettings();
                    break;
                case LinerRegressionStrategyName:
                    setting = new LinerRegStrategySettings();
                    break;
                case ASCTrendStrategyName:
                    setting = new ASCTrendStrategySettings();
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

#region Using declarations
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Xml.Serialization;
using System.Globalization;
using NinjaTrader.Cbi;
using NinjaTrader.Data;
using NinjaTrader.Gui.Chart;
#endregion

// This namespace holds all indicators and is required. Do not change it.
namespace NinjaTrader.Indicator
{
    /// <summary>
    /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
    /// </summary>
    public class iMyMidasCP : Indicator
    {
        #region Variables
		private DataSeries MP, pv, M;
		private double denom;
		
		private double cumVol = 0;
		private double startCumVol = 0;
		
		private double cumPV = 0;
		private double startCumPV = 0;
		private Color      colorDown = Color.Red;
        private Color      colorUp   = Color.Green;
		private Color      colorNe   = Color.Orange;
		private int					period	= 12;
        #endregion

        /// <summary>
        /// This method is used to configure the indicator and is called once before any bar data is loaded.
        /// </summary>
        protected override void Initialize()
        {
			Add(new Plot(Color.DarkOrange, PlotStyle.Line, "Midas"));
			CalculateOnBarClose	= true;
            Overlay				= true;
            PriceTypeSupported	= false;
			MP = new DataSeries(this);
			pv = new DataSeries(this);
			M = new DataSeries(this);
        }

        /// <summary>
        /// Called on each bar update event (incoming tick)
        /// </summary>
        protected override void OnBarUpdate()
        {
			if (CurrentBar < period+1)
				M.Set(Median[0]);
			else
			{
				MP.Set(0);
				pv.Set(0);
				cumVol = 0;
				cumPV = 0;
				//M.Set(0);
				for (int count = period; count > -1; count--)
				{
					MP.Set((High[count] + Low[count]) / 2);
					pv.Set(MP[count] * Volume[count]);
					
					cumVol += Volume[count];
					cumPV += pv[0];
					
					if (count == period)
					{
						startCumVol = cumVol;
						startCumPV	= cumPV;
						continue;
					}
					if ((cumVol - startCumVol) == 0)
						denom = 1;
					else
						denom = cumVol - startCumVol;
					
					M.Set((cumPV - startCumPV) / denom);
				}
			}
			Midas.Set(M[0]);
			if (Rising(Midas))
				PlotColors[0][0] = ColorUp;
			else if (Falling(Midas))
				PlotColors[0][0] = ColorDown;
			else
				PlotColors[0][0] = ColorNe;
        }

        #region Properties
        [Browsable(false)]	// this line prevents the data series from being displayed in the indicator properties dialog, do not remove
        [XmlIgnore()]		// this line ensures that the indicator can be saved/recovered as part of a chart template, do not remove
        public DataSeries Midas
        {
            get { return Values[0]; }
        }
		[Description("Numbers of bars used for calculations")]
		[GridCategory("Parameters")]
		public int Period
		{
			get { return period; }
			set { period = Math.Max(2, value); }
		}
		[Description("UpTrend Color")]
        [Category("Colors")]
        public Color ColorUp
        {
            get { return colorUp; }
            set { colorUp = value; }
        }
        [Browsable(false)]
        public string UpColorSerialize
        {
            get { return Gui.Design.SerializableColor.ToString(colorUp); }
            set { colorUp = Gui.Design.SerializableColor.FromString(value); }
        }
        [Description("DownTrend Color")]
        [Category("Colors")]
        public Color ColorDown
        {
            get { return colorDown; }
            set { colorDown = value; }
        }
        [Browsable(false)]
        public string DownColorSerialize
        {
            get { return Gui.Design.SerializableColor.ToString(colorDown); }
            set { colorDown = Gui.Design.SerializableColor.FromString(value); }
        }
		[Description("DownTrend Color")]
        [Category("Colors")]
        public Color ColorNe
        {
            get { return colorNe; }
            set { colorNe = value; }
        }
        [Browsable(false)]
        public string NeColorSerialize
        {
            get { return Gui.Design.SerializableColor.ToString(colorNe); }
            set { colorNe = Gui.Design.SerializableColor.FromString(value); }
        }
        #endregion
    }
}

#region NinjaScript generated code. Neither change nor remove.
// This namespace holds all indicators and is required. Do not change it.
namespace NinjaTrader.Indicator
{
    public partial class Indicator : IndicatorBase
    {
        private iMyMidasCP[] cacheiMyMidasCP = null;

        private static iMyMidasCP checkiMyMidasCP = new iMyMidasCP();

        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        public iMyMidasCP iMyMidasCP(int period)
        {
            return iMyMidasCP(Input, period);
        }

        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        public iMyMidasCP iMyMidasCP(Data.IDataSeries input, int period)
        {
            if (cacheiMyMidasCP != null)
                for (int idx = 0; idx < cacheiMyMidasCP.Length; idx++)
                    if (cacheiMyMidasCP[idx].Period == period && cacheiMyMidasCP[idx].EqualsInput(input))
                        return cacheiMyMidasCP[idx];

            lock (checkiMyMidasCP)
            {
                checkiMyMidasCP.Period = period;
                period = checkiMyMidasCP.Period;

                if (cacheiMyMidasCP != null)
                    for (int idx = 0; idx < cacheiMyMidasCP.Length; idx++)
                        if (cacheiMyMidasCP[idx].Period == period && cacheiMyMidasCP[idx].EqualsInput(input))
                            return cacheiMyMidasCP[idx];

                iMyMidasCP indicator = new iMyMidasCP();
                indicator.BarsRequired = BarsRequired;
                indicator.CalculateOnBarClose = CalculateOnBarClose;
#if NT7
                indicator.ForceMaximumBarsLookBack256 = ForceMaximumBarsLookBack256;
                indicator.MaximumBarsLookBack = MaximumBarsLookBack;
#endif
                indicator.Input = input;
                indicator.Period = period;
                Indicators.Add(indicator);
                indicator.SetUp();

                iMyMidasCP[] tmp = new iMyMidasCP[cacheiMyMidasCP == null ? 1 : cacheiMyMidasCP.Length + 1];
                if (cacheiMyMidasCP != null)
                    cacheiMyMidasCP.CopyTo(tmp, 0);
                tmp[tmp.Length - 1] = indicator;
                cacheiMyMidasCP = tmp;
                return indicator;
            }
        }
    }
}

// This namespace holds all market analyzer column definitions and is required. Do not change it.
namespace NinjaTrader.MarketAnalyzer
{
    public partial class Column : ColumnBase
    {
        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        [Gui.Design.WizardCondition("Indicator")]
        public Indicator.iMyMidasCP iMyMidasCP(int period)
        {
            return _indicator.iMyMidasCP(Input, period);
        }

        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        public Indicator.iMyMidasCP iMyMidasCP(Data.IDataSeries input, int period)
        {
            return _indicator.iMyMidasCP(input, period);
        }
    }
}

// This namespace holds all strategies and is required. Do not change it.
namespace NinjaTrader.Strategy
{
    public partial class Strategy : StrategyBase
    {
        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        [Gui.Design.WizardCondition("Indicator")]
        public Indicator.iMyMidasCP iMyMidasCP(int period)
        {
            return _indicator.iMyMidasCP(Input, period);
        }

        /// <summary>
        /// This is the MIDAS indicator as described in the July 2010 issue of Stocks & Commodities.
        /// </summary>
        /// <returns></returns>
        public Indicator.iMyMidasCP iMyMidasCP(Data.IDataSeries input, int period)
        {
            if (InInitialize && input == null)
                throw new ArgumentException("You only can access an indicator with the default input/bar series from within the 'Initialize()' method");

            return _indicator.iMyMidasCP(input, period);
        }
    }
}
#endregion

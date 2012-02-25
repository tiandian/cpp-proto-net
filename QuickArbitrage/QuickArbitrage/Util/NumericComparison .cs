using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Main.Util
{
    static class NumericComparison
    {
        public static bool equals(this float source, float destination)
        {
            float diff = Math.Abs(source - destination);
            return diff < 0.0001;
        }

        public static bool equals(this double source, double destination)
        {
            double diff = Math.Abs(source - destination);
            return diff < 0.0001;
        }
    }
}

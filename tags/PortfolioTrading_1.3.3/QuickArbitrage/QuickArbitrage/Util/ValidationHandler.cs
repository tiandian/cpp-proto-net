using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Main.Util
{
    public class ValidationHandler
    {
        private Dictionary<string, string> BrokenRules { get; set; }
        public ValidationHandler()
        {
            BrokenRules = new Dictionary<string, string>();
        }
        public string this[string property]
        {
            get
            {
                return this.BrokenRules[property];
            }
        }
        public bool BrokenRuleExists(string property)
        {
            return BrokenRules.ContainsKey(property);
        }
        public bool ValidateRule(string property, string message, Func<bool> ruleCheck)
        {
            System.Diagnostics.Debug.WriteLine("IN THE VALIDATION");
            if (!ruleCheck())
            {
                this.BrokenRules.Add(property, message);
                return false;
            }
            else
            {
                RemoveBrokenRule(property);
                return true;
            }
        }
        public void RemoveBrokenRule(string property)
        {
            if (this.BrokenRules.ContainsKey(property))
            {
                this.BrokenRules.Remove(property);
            }
        }
    }
}

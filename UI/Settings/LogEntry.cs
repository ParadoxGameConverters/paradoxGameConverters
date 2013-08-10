using System;

namespace Converter.UI.Settings
{
    public class LogEntry
    {
        private string logText;
        private DateTime logStamp;

        public LogEntry(string text)
        {
            this.logText = text;
            this.logStamp = DateTime.Now;
        }

        public string LogText
        {
            get
            {
                return this.logText;
            }
        }

        public DateTime LogStamp
        {
            get
            {
                return this.logStamp;
            }
        }

        public override string ToString()
        {
            return this.LogStamp + " : " + this.LogText;
        }
    }
}

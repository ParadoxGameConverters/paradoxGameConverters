using System;
using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public class LogEntry
    {
        private string logText;
        private DateTime logStamp;

        public LogEntry(string text, LogEntrySeverity severity, LogEntrySource source)
        {
            this.logText = text;
            this.logStamp = DateTime.Now;
            this.Severity = severity;
            this.Source = source;
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

        public LogEntrySource Source { get; set; }

        public LogEntrySeverity Severity { get; set; }

        public override string ToString()
        {
            return this.LogStamp + " : " + this.LogText;
        }
    }
}

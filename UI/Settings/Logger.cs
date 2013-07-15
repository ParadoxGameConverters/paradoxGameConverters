using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace Converter.UI.Settings
{
    public class Logger
    {
        private ReadOnlyObservableCollection<LogEntry> logEntries;
        private ObservableCollection<LogEntry> internalLogEntries;

        public ReadOnlyObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.logEntries ?? (this.logEntries = new ReadOnlyObservableCollection<LogEntry>(this.InternalLogEntries));
            }
        }

        private ObservableCollection<LogEntry> InternalLogEntries
        {
            get
            {
                return this.internalLogEntries ?? (this.internalLogEntries = new ObservableCollection<LogEntry>());
            }
        }

        public void AddLogEntry(LogEntry logEntry)
        {
            this.InternalLogEntries.Add(logEntry);
        }
    }
}

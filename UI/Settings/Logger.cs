using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace Converter.UI.Settings
{
    /// <summary>
    /// This is the logger object - that controls the list of log entries, and allows new entries to be added.
    /// </summary>
    public class Logger
    {
        private ReadOnlyObservableCollection<LogEntry> logEntries;
        private ObservableCollection<LogEntry> internalLogEntries;

        /// <summary>
        /// Gets the log entries.
        /// </summary>
        /// <value>
        /// The log entries.
        /// </value>
        public ReadOnlyObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.logEntries ?? (this.logEntries = new ReadOnlyObservableCollection<LogEntry>(this.InternalLogEntries));
            }
        }

        /// <summary>
        /// Gets the internal log entries.
        /// </summary>
        /// <value>
        /// The internal log entries.
        /// </value>
        private ObservableCollection<LogEntry> InternalLogEntries
        {
            get
            {
                return this.internalLogEntries ?? (this.internalLogEntries = new ObservableCollection<LogEntry>());
            }
        }

        /// <summary>
        /// Adds the log entry.
        /// </summary>
        /// <param name="logEntry">The log entry.</param>
        public void AddLogEntry(LogEntry logEntry)
        {
            this.InternalLogEntries.Add(logEntry);
        }
    }
}

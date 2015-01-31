using System;

namespace Frontend.Core.Logging
{
    /// <summary>
    /// A logentry is, well, an entry in the log that is outputted to screen.
    /// </summary>
    public class LogEntry
    {
        private string logText;
        private DateTime logStamp;
        
        /// <summary>
        /// Initializes a new instance of the <see cref="LogEntry"/> class.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="severity">The severity.</param>
        /// <param name="source">The source.</param>
        /// <param name="path">The path where something happened</param>
        public LogEntry(string text, LogEntrySeverity severity, LogEntrySource source, string path)
        {
            this.logText = text;
            this.logStamp = DateTime.Now;
            this.Severity = severity;
            this.Source = source;
            this.Path = path;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="LogEntry"/> class.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="severity">The severity.</param>
        /// <param name="source">The source.</param>
        public LogEntry(string text, LogEntrySeverity severity, LogEntrySource source)
            : this(text, severity, source, string.Empty)
        {
        }

        /// <summary>
        /// Gets the log text.
        /// </summary>
        /// <value>
        /// The log text.
        /// </value>
        public string LogText
        {
            get
            {
                return this.logText;
            }
        }

        /// <summary>
        /// Gets the log stamp.
        /// </summary>
        /// <value>
        /// The log stamp.
        /// </value>
        public DateTime LogStamp
        {
            get
            {
                return this.logStamp;
            }
        }

        /// <summary>
        /// Gets or sets the source.
        /// </summary>
        /// <value>
        /// The source.
        /// </value>
        public LogEntrySource Source { get; set; }

        /// <summary>
        /// Gets or sets the severity.
        /// </summary>
        /// <value>
        /// The severity.
        /// </value>
        public LogEntrySeverity Severity { get; set; }

        /// <summary>
        /// Gets or sets the path to where something happened. Can be both file and folder
        /// </summary>
        public string Path { get; set; }

        /// <summary>
        /// Returns a <see cref="System.String" /> that represents this instance.
        /// </summary>
        /// <returns>
        /// A <see cref="System.String" /> that represents this instance.
        /// </returns>
        public override string ToString()
        {
            return this.LogStamp + " : " + this.LogText;
        }
    }
}

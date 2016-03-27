using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Converter.UI.Enums
{
    /// <summary>
    /// Used to differentiate between the source of a particular log entry.
    /// </summary>
    public enum LogEntrySource
    {
        /// <summary>
        /// The UI (Frontend) caused this log entry
        /// </summary>
        UI,

        /// <summary>
        /// The converter itself caused this log entry
        /// </summary>
        Converter
    }
}

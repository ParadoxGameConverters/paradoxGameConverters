using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Converter.UI.Enums
{
    /// <summary>
    /// Enumerates the various degrees of severity a single log entry could have
    /// </summary>
    public enum LogEntrySeverity
    {
        /// <summary>
        /// The information
        /// </summary>
        Info,

        /// <summary>
        /// The warning
        /// </summary>
        Warning,

        /// <summary>
        /// The error
        /// </summary>
        Error
    }
}

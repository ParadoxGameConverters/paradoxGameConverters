using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Preferences.Interfaces
{
    public interface IDatePreference : IPreference<DateTime>
    {
        /// <summary>
        /// Gets or sets the date format.
        /// </summary>
        string DateFormat { get; set; }
    }
}

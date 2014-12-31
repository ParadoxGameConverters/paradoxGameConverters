using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Events.EventArgs
{
    /// <summary>
    /// This enum indicates whether the fired event indicates an add or remove step operation
    /// </summary>
    /// <remarks>
    /// It's worth noting that both operations are bulk operations - they affect multiple steps
    /// </remarks>
    public enum PreferenceOperation
    {
        // Add one to many steps, each found in the NewSteps collection
        AddSteps,

        // Clear all the PREFERENCE steps. Not the welcome and path picker steps - those should always be available.
        // A preference step is one where the user can set options specific to one particular converter
        Clear
    }
}

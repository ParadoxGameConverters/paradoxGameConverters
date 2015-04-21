using System.Collections.Generic;
using Frontend.Core.Navigation;

namespace Frontend.Core.Events.EventArgs
{
    public class PreferenceStepOperationArgs
    {
        /// <summary>
        ///     Event arguments for adding and removing preference steps.
        ///     This is always done as bulk operations (so either all preference steps are removed, or 1-many new steps are added.)
        /// </summary>
        /// <param name="operation"></param>
        /// <param name="newSteps"></param>
        public PreferenceStepOperationArgs(PreferenceOperation operation, IList<IStep> newSteps)
        {
            Operation = operation;
            NewSteps = newSteps;
        }

        public PreferenceOperation Operation { get; private set; }

        /// <summary>
        ///     The new step
        /// </summary>
        public IList<IStep> NewSteps { get; private set; }
    }
}
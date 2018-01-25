using System.Collections.Generic;
using Frontend.Core.Logging;

namespace Frontend.Core.Converting.Operations
{
    public class OperationResult
    {
        public OperationResultState State;

        public OperationResult()
        {
            State = OperationResultState.Success;
            LogEntries = new List<LogEntry>();
        }

        public IList<LogEntry> LogEntries { get; private set; }
    }
}
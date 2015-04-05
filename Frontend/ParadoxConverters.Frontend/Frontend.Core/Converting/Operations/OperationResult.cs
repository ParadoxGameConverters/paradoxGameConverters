using Frontend.Core.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public class OperationResult
    {
        private IList<LogEntry> logEntries;

        public OperationResult()
        {
            this.State = OperationResultState.Success;
            this.logEntries = new List<LogEntry>();
        }

        public OperationResultState State;

        public IList<LogEntry> LogEntries
        {
            get
            {
                return this.logEntries;
            }
        }
    }
}

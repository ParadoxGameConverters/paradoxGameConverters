using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public enum AggregateOperationsResult
    {
        NotStarted,

        InProgress,

        Canceled,

        CompletedSuccessfully,

        CompletedWithWarnings,

        CompletedWithErrors
    }
}

using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public interface IOperationProcessor
    {
        Task<AggregateOperationsResult> ProcessQueue(IEnumerable<IOperationViewModel> operations,
            CancellationToken cancellationToken);
    }
}
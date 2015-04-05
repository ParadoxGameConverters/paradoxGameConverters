using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
namespace Frontend.Core.Converting.Operations
{
    public interface IOperationProcessor
    {
        Task<int> ProcessQueue(IEnumerable<IOperationViewModel> operations, IProgress<int> progress, CancellationToken cancellationToken);
    }
}

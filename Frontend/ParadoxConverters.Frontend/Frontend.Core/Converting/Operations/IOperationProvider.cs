using System.Collections.Generic;

namespace Frontend.Core.Converting.Operations
{
    public interface IOperationProvider
    {
        IEnumerable<IOperationViewModel> Operations { get; }
        void AddOperation(IOperation operation);
    }
}
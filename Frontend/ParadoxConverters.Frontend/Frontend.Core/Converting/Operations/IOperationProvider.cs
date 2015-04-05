using System;
using System.Collections.Generic;

namespace Frontend.Core.Converting.Operations
{
    public interface IOperationProvider
    {
        void AddOperation(IOperation operation);
        IEnumerable<IOperationViewModel> Operations { get; }
    }
}

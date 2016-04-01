using System.Collections.Generic;
using System.Linq;
using Caliburn.Micro;

namespace Frontend.Core.Converting.Operations
{
    public class OperationProvider : IOperationProvider
    {
        private readonly IEventAggregator eventAggregator;
        private readonly IList<IOperationViewModel> registeredOperations;

        public OperationProvider(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
            registeredOperations = new List<IOperationViewModel>();
        }

        public IEnumerable<IOperationViewModel> Operations
        {
            get { return registeredOperations.Where(operation => operation.CanRun()); }
        }

        public void RegisterOperation(IOperation operation)
        {
            var operationViewModel = new OperationViewModel(eventAggregator);
            operationViewModel.Load(operation);
            registeredOperations.Add(operationViewModel);
        }
    }
}
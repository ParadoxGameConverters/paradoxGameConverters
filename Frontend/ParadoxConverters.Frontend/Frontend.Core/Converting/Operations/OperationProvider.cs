using System.Collections.Generic;
using Caliburn.Micro;

namespace Frontend.Core.Converting.Operations
{
    public class OperationProvider : IOperationProvider
    {
        private readonly IEventAggregator eventAggregator;
        private readonly IList<IOperationViewModel> operations;

        public OperationProvider(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
            operations = new List<IOperationViewModel>();
        }

        public IEnumerable<IOperationViewModel> Operations
        {
            get { return operations; }
        }

        public void AddOperation(IOperation operation)
        {
            var operationViewModel = new OperationViewModel(eventAggregator);
            operationViewModel.Load(operation);
            operations.Add(operationViewModel);
        }
    }
}
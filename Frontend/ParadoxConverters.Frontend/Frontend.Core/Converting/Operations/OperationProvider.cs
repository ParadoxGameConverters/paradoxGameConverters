using Caliburn.Micro;
using System.Collections.Generic;

namespace Frontend.Core.Converting.Operations
{
    public class OperationProvider : IOperationProvider
    {
        private IList<IOperationViewModel> operations;
        private IEventAggregator eventAggregator;

        public OperationProvider(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
            this.operations = new List<IOperationViewModel>();
        }

        public IEnumerable<IOperationViewModel> Operations
        {
            get
            {
                return this.operations;
            }
        }

        public void AddOperation(IOperation operation)
        {
            var operationViewModel = new OperationViewModel(this.eventAggregator);
            operationViewModel.Load(operation);
            this.operations.Add(operationViewModel);
        }
    }
}

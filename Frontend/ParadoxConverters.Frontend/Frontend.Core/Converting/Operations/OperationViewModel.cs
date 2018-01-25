using Caliburn.Micro;
using Frontend.Core.Common;

namespace Frontend.Core.Converting.Operations
{
    public class OperationViewModel : ViewModelBase, IOperationViewModel
    {
        private IOperation operation;
        private OperationState state;

        public OperationViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        public void Load(IOperation operation)
        {
            Description = operation.Description;
            this.operation = operation;
        }

        public OperationResult Process()
        {
            return operation.Process();
        }

        public bool CanRun()
        {
            return operation.CanRun();
        }

        public string Description { get; private set; }

        public OperationState State
        {
            get { return state; }

            set
            {
                if (state == value)
                {
                    return;
                }

                state = value;
                NotifyOfPropertyChange(() => State);
            }
        }
    }
}
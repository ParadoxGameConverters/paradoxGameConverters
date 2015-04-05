using Caliburn.Micro;
using Frontend.Core.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public class OperationViewModel : ViewModelBase, IOperationViewModel
    {
        private IOperation operation;
        private string description;
        private OperationState state;

        public OperationViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        public void Load(IOperation operation)
        {
            this.description = operation.Description;
            this.operation = operation;
        }

        public async Task<OperationResult> Process()
        {
            return await this.operation.Process();
        }

        public bool CanRun()
        {
            return this.operation.CanRun();
        }

        public string Description
        {
            get
            {
                return this.description;
            }
        }

        public OperationState State
        {
            get
            {
                return this.state;
            }

            set
            {
                if (this.state == value)
                {
                    return;
                }

                this.state = value;
                this.NotifyOfPropertyChange(() => this.State);
            }
        }
    }
}

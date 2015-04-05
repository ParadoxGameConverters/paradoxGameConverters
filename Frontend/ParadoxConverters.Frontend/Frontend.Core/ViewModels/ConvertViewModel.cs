using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Common;
using Frontend.Core.Converting;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Proxies;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Input;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    /// This viewmodel for the converter view
    /// </summary>
    public class ConvertViewModel : StepViewModelBase, IConvertViewModel
    {
        private ICommand runOperationsCommand;
        private ICommand cancelCommand;
        private IOperationProvider operationProvider;
        private int progressInPercent;
        private bool isBusy;
        private CancellationTokenSource cancellationTokenSource;

        /// <summary>
        /// Initializes a new instance of the <see cref="ConvertViewModel"/> class.
        /// </summary>
        /// <param name="eventAggregator">The event aggregator.</param>
        /// <param name="options">The options.</param>
        public ConvertViewModel(IEventAggregator eventAggregator, IConverterOptions options, IOperationProvider operationProvider)
            : base(eventAggregator, options)
        {
            this.operationProvider = operationProvider;

            this.operationProvider.AddOperation(new SaveConfigurationOperation(
                options, 
                new DirectoryHelper(), 
                new FileProxy(), 
                new FolderProxy(), 
                new OutputConfigurationFileHelper(new FileProxy())));

            this.operationProvider.AddOperation(new ExtractSaveOperation());

            this.operationProvider.AddOperation(new ConvertSaveOperation(this.Options, new FileProxy(), new DirectoryHelper()));

            this.operationProvider.AddOperation(new CopyModOperation());

            this.cancellationTokenSource = new CancellationTokenSource();
        }

        public ICommand RunOperationsCommand
        {
            get
            {
                return this.runOperationsCommand ?? (this.runOperationsCommand = new RunOperationsCommand(
                    this.EventAggregator, 
                    this.Options, 
                    new OperationProcessor(this.EventAggregator), 
                    this.operationProvider,
                    percent => this.UpdateProgress(percent),
                    this.cancellationTokenSource.Token
                    ));
            }
        }

        public ICommand CancelCommand
        {
            get
            {
                return this.cancelCommand ?? (this.cancelCommand = new CancelConvertingCommand(this.EventAggregator, this.cancellationTokenSource));
            }
        }

        public int ProgressInPercent
        {
            get
            {
                return this.progressInPercent;
            }

            set
            {
                if (this.progressInPercent == value)
                {
                    return;
                }

                this.progressInPercent = value;
                this.NotifyOfPropertyChange(() => this.ProgressInPercent);
            }
        }
        
        public bool IsBusy
        {
            get
            {
                return this.isBusy;
            }

            set
            {
                if (this.isBusy == value)
                {
                    return;
                }

                this.isBusy = value;
                this.NotifyOfPropertyChange(() => this.IsBusy);
            }
        }

        public IEnumerable<IOperationViewModel> Operations
        {
            get
            {
                return this.operationProvider.Operations;
            }
        }

        /// <summary>
        /// Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool IsValid
        {
            get
            {
                return true;
            }
        }

        private void UpdateProgress(int percent)
        {
            if (!this.IsBusy)
            {
                this.StartProgress();
            }

            this.ProgressInPercent = percent;

            if (percent == 100)
            {
                this.StopProgress();
            }
        }

        private void StartProgress()
        {
            this.IsBusy = true;
        }

        private void StopProgress()
        {
            this.IsBusy = false;
        }
    }
}

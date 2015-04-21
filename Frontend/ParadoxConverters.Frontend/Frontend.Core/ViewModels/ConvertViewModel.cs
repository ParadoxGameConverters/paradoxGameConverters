using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Converting.Operations.ConvertSave;
using Frontend.Core.Converting.Operations.CopyMod;
using Frontend.Core.Converting.Operations.ExtractSave;
using Frontend.Core.Converting.Operations.SaveConfiguration;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
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
        private readonly IOperationProvider operationProvider;
        private int progressInPercent;
        private bool isBusy;
        private CancellationTokenSource cancellationTokenSource;
        private CancellationTokenRegistration tokenRegistration;
        private Func<CancellationTokenSource> getOrCreateCancellationTokenSource;

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
                new OutputConfigurationFileHelper(new FileProxy(), new EnvironmentProxy())));

            this.operationProvider.AddOperation(new ExtractSaveOperation(
                this.Options, 
                new CompressedSaveChecker(), 
                new ZipFileHelper(
                    new ZipFileProxy(),
                    new FileProxy(), 
                    new MessageBoxProxy()), 
                    new EnvironmentProxy()));

            this.operationProvider.AddOperation(new ConvertSaveOperation(this.Options, new FileProxy(), new DirectoryHelper()));

            this.operationProvider.AddOperation(new CopyModOperation(this.EventAggregator, this.Options));

            this.getOrCreateCancellationTokenSource = new Func<CancellationTokenSource>(() =>
            {
                return this.CancellationTokenSource;
            });
        }

        public ICommand RunOperationsCommand
        {
            get
            {
                return this.runOperationsCommand ?? (this.runOperationsCommand = new RunOperationsCommand(
                    this.EventAggregator, 
                    new OperationProcessor(this.EventAggregator), 
                    this.operationProvider,
                    percent => this.UpdateProgress(percent),
                    getOrCreateCancellationTokenSource
                    ));
            }
        }

        public ICommand CancelCommand
        {
            get
            {
                return this.cancelCommand ?? (this.cancelCommand = new CancelConvertingCommand(this.EventAggregator, this.getOrCreateCancellationTokenSource));
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

        private CancellationTokenSource CancellationTokenSource
        {
            get
            {
                if (this.cancellationTokenSource == null)
                {
                    this.cancellationTokenSource = new CancellationTokenSource();
                    this.tokenRegistration = this.cancellationTokenSource.Token.Register(this.OperationCancelledCallback);
                }

                return this.cancellationTokenSource;
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
            this.tokenRegistration.Dispose();
            this.cancellationTokenSource.Dispose();
            this.cancellationTokenSource = null;
        }

        private void OperationCancelledCallback()
        {
            this.Operations.Where(o => o.State == OperationState.InProgress || o.State == OperationState.NotStarted)
                                .ForEach(o => o.State = OperationState.Cancelled);
            this.StopProgress();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Windows.Input;
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

namespace Frontend.Core.ViewModels
{
    /// <summary>
    ///     This viewmodel for the converter view
    /// </summary>
    public class ConvertViewModel : StepViewModelBase, IConvertViewModel
    {
        private readonly Func<CancellationTokenSource> getOrCreateCancellationTokenSource;
        private readonly IOperationProvider operationProvider;
        private ICommand cancelCommand;
        private CancellationTokenSource cancellationTokenSource;
        private bool isBusy;
        private ICommand runOperationsCommand;
        private CancellationTokenRegistration tokenRegistration;

        /// <summary>
        ///     Initializes a new instance of the <see cref="ConvertViewModel" /> class.
        /// </summary>
        /// <param name="eventAggregator">The event aggregator.</param>
        /// <param name="options">The options.</param>
        public ConvertViewModel(IEventAggregator eventAggregator, IConverterOptions options,
            IOperationProvider operationProvider)
            : base(eventAggregator, options)
        {
            this.operationProvider = operationProvider;

            this.operationProvider.RegisterOperation(new SaveConfigurationOperation(
                options,
                new DirectoryHelper(),
                new FileProxy(),
                new FolderProxy(),
                new OutputConfigurationFileHelper(new FileProxy(), new EnvironmentProxy())));

            this.operationProvider.RegisterOperation(new ExtractSaveOperation(
                Options,
                new CompressedSaveChecker(),
                new ZipFileHelper(
                    new ZipFileProxy(),
                    new FileProxy(),
                    new MessageBoxProxy()),
                new EnvironmentProxy()));

            this.operationProvider.RegisterOperation(new ConvertSaveOperation(Options, new FileProxy(), new DirectoryHelper()));

            this.operationProvider.RegisterOperation(new CopyModOperation(Options));

            getOrCreateCancellationTokenSource = () => { return CancellationTokenSource; };
        }

        public ICommand RunOperationsCommand
        {
            get
            {
                return runOperationsCommand ?? (runOperationsCommand = new RunOperationsCommand(
                    new OperationProcessor(EventAggregator),
                    operationProvider,
                    StartProgress,
                    StopProgress,
                    getOrCreateCancellationTokenSource
                    ));
            }
        }

        public ICommand CancelCommand
        {
            get
            {
                return cancelCommand ??
                       (cancelCommand = new CancelConvertingCommand(EventAggregator, getOrCreateCancellationTokenSource));
            }
        }

        public bool IsBusy
        {
            get { return isBusy; }

            set
            {
                if (isBusy == value)
                {
                    return;
                }

                isBusy = value;
                NotifyOfPropertyChange(() => IsBusy);
            }
        }

        public IEnumerable<IOperationViewModel> Operations
        {
            get { return operationProvider.Operations; }
        }

        private CancellationTokenSource CancellationTokenSource
        {
            get
            {
                if (cancellationTokenSource == null)
                {
                    cancellationTokenSource = new CancellationTokenSource();
                    tokenRegistration = cancellationTokenSource.Token.Register(OperationCancelledCallback);
                }

                return cancellationTokenSource;
            }
        }

        /// <summary>
        ///     Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool IsValid
        {
            get { return true; }
        }

        private void StartProgress()
        {
            IsBusy = true;
        }

        private void StopProgress()
        {
            IsBusy = false;
            tokenRegistration.Dispose();
            cancellationTokenSource.Dispose();
            cancellationTokenSource = null;
        }

        private void OperationCancelledCallback()
        {
            Operations.Where(o => o.State == OperationState.InProgress || o.State == OperationState.NotStarted)
                .ForEach(o => o.State = OperationState.Cancelled);
            StopProgress();
        }
    }
}
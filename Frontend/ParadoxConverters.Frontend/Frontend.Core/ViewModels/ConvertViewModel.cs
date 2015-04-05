using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Converting;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Proxies;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Windows.Input;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    /// This viewmodel for the converter view
    /// </summary>
    public class ConvertViewModel : StepViewModelBase, IConvertViewModel
    {
        private ICommand runOperationsCommand;
        private IOperationProvider operationProvider;
        private int progressInPercent;
        private Action<int> updateUIAction;

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
                    this.UpdateUIAction
                    ));
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
                    this.progressInPercent = value;
                    this.NotifyOfPropertyChange(() => this.ProgressInPercent);
                }
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

        private Action<int> UpdateUIAction
        {
            get
            {
                if (this.updateUIAction == null)
                {
                    this.updateUIAction = percent => this.ProgressInPercent = percent;
                }

                return this.updateUIAction;
            }
        }
    }
}

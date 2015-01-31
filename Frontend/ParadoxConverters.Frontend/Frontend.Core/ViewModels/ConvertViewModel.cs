using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.ViewModels.Interfaces;
using System.Windows.Input;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    /// This viewmodel for the converter view
    /// </summary>
    public class ConvertViewModel : StepViewModelBase, IConvertViewModel
    {
        private ICommand convertCommand;

        /// <summary>
        /// Initializes a new instance of the <see cref="ConvertViewModel"/> class.
        /// </summary>
        /// <param name="eventAggregator">The event aggregator.</param>
        /// <param name="options">The options.</param>
        public ConvertViewModel(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        /// <summary>
        /// Gets the convert command.
        /// </summary>
        /// <value>
        /// The convert command.
        /// </value>
        public ICommand ConvertCommand
        {
            get
            {
                return this.convertCommand ?? (this.convertCommand = new ConvertCommand(this.EventAggregator, this.Options));
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
    }
}

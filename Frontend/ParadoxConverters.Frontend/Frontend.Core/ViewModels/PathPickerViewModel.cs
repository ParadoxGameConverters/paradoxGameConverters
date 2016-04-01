using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.ViewModels.Interfaces;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    /// The PathPickerViewModel lets the user select paths to various stuff the converter needs to know where to find.
    /// </summary>
    public class PathPickerViewModel : StepViewModelBase, IPathPickerViewModel
    {
        private ICommand openFileCommand;
        private ICommand openFolderCommand;

        /// <summary>
        /// Initializes a new instance of the <see cref="PathPickerViewModel"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        /// <param name="view">The view.</param>
        public PathPickerViewModel(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        #region [ Properties ]

        /// <summary>
        /// Gets the open save game command.
        /// </summary>
        /// <value>
        /// The open save game command.
        /// </value>
        public ICommand OpenFileCommand
        {
            get
            {
                return this.openFileCommand ?? (this.openFileCommand = new OpenFileCommand(this.EventAggregator, this.Options));
            }
        }

        /// <summary>
        /// Gets the open folder command.
        /// </summary>
        /// <value>
        /// The open folder command.
        /// </value>
        public ICommand OpenFolderCommand
        {
            get
            {
                return this.openFolderCommand ?? (this.openFolderCommand = new OpenFolderCommand(this.EventAggregator, this.Options));
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
                return this.Options.CurrentConverter.RequiredItems.All(i => i.IsValid);
            }
        }

        #endregion

        #region [ Methods ]

        protected override void OnUnloading()
        {
            this.Options.CurrentConverter.RequiredItems.ForEach(i => i.PropertyChanged -= this.Item_PropertyChanged);
            base.OnUnloading();
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);
            this.Options.CurrentConverter.RequiredItems.ForEach(i => i.PropertyChanged += this.Item_PropertyChanged);
        }

        private void Item_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName.Equals("SelectedValue"))
            {
                this.NotifyOfPropertyChange(() => this.IsValid);
            }
        }

        #endregion
    }
}

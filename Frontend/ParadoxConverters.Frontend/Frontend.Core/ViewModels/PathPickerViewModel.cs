using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.IO;
using System.Windows.Input;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    /// The PathPickerViewModel lets the user select paths to various stuff the converter needs to know where to find.
    /// </summary>
    public class PathPickerViewModel : StepViewModelBase, IPathPickerViewModel
    {
        private ICommand openSaveGameCommand;
        private ICommand openConverterCommand;
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
        /// Gets the open converter command.
        /// </summary>
        /// <value>
        /// The open converter command.
        /// </value>
        public ICommand OpenConverterCommand
        {
            get
            {
                return this.openConverterCommand ?? (this.openConverterCommand = new OpenConverterCommand(this.EventAggregator, this.Options));
            }
        }

        /// <summary>
        /// Gets the open save game command.
        /// </summary>
        /// <value>
        /// The open save game command.
        /// </value>
        public ICommand OpenSaveGameCommand
        {
            get
            {
                return this.openSaveGameCommand ?? (this.openSaveGameCommand = new OpenSaveGameCommand(this.EventAggregator, this.Options));
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
        /// Gets or sets the source save game path.
        /// </summary>
        /// <value>
        /// The source save game path.
        /// </value>
        public string SourceSaveGamePath
        {
            get
            {
                return this.Options.CurrentConverter.AbsoluteSourceSaveGamePath;
            }

            set
            {
                if (this.Options.CurrentConverter.AbsoluteSourceSaveGamePath == value)
                {
                    return;
                }

                this.Options.CurrentConverter.AbsoluteSourceSaveGamePath = value;
                this.NotifyOfPropertyChange(() => this.SourceSaveGamePath);
            }
        }

        #endregion

        #region [ Methods ]

        /// <summary>
        /// Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool CanValidate()
        {
            bool isSourceGamePathValid = Directory.Exists(this.Options.CurrentConverter.SourceGame.AbsoluteInstallationPath);
            bool isTargetGamePathValid = Directory.Exists(this.Options.CurrentConverter.TargetGame.AbsoluteInstallationPath);
            bool isSaveGamePathValid = File.Exists(this.Options.CurrentConverter.AbsoluteSourceSaveGamePath);

            return isSourceGamePathValid && isTargetGamePathValid && isSaveGamePathValid;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        protected override void OnLoaded(object parameter)
        {
            //if (this.Options == null || this.Options.CurrentConverter == null)
            //{
            //    return;
            //}

            //// If the directory where the converter should be, doesn't exist, create it. 
            //if (!Directory.Exists(this.Options.CurrentConverter.AbsoluteConverterPath))
            //{
            //    Directory.CreateDirectory(this.Options.CurrentConverter.AbsoluteConverterPath);
            //}

            // If the converter .exe happens to be in the same directory as the UI + relevant subfolder, there is no reason to ask the user to set it. So let's just do it for them.
            var converterLocation = Path.Combine(this.Options.CurrentConverter.AbsoluteConverterPath, this.Options.CurrentConverter.ConverterExeName); 
            if (File.Exists(converterLocation))
            {
                this.Options.CurrentConverter.AbsoluteConverterPath = converterLocation;
            }

            // Add one step per category
            //foreach()
        }

        #endregion
    }
}

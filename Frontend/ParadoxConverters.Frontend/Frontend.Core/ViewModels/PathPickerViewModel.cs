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
            this.BuildConversionOptions();
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

        protected override void OnLoaded(object parameter)
        {
            // If the directory where the converter should be, doesn't exist, create it. 
            if (!Directory.Exists(this.Options.CurrentConverter.AbsoluteConverterPath))
            {
                Directory.CreateDirectory(this.Options.CurrentConverter.AbsoluteConverterPath);

                // Then extract the 
            }
        }

        //protected override void OnLoaded(object parameter)
        //{
        //    // If the converter .exe happens to be in the same directory as the UI, there is no reason to ask the user to find it anyway. So let's just do it for them.
        //    var converterLocation = Path.Combine(Environment.CurrentDirectory, "CK2ToEU3.exe"); //TODO:FIX HARD CODING
        //    if (File.Exists(converterLocation))
        //    {
        //        this.Options.CurrentConverter.AbsoluteConverterPath = converterLocation;
        //    }
        //}

        /// <summary>
        /// Builds the conversion options.
        /// </summary>
        private void BuildConversionOptions()
        {
            //TODO: Finding installation paths makes little sense if the game isn't installed. Should probably be fixed
            //this.VerifyInstallation(this.Options.SourceGame);
            //this.VerifyInstallation(this.Options.TargetGame);
            //this.FindGameInstallationPath(this.Options.SourceGame);
            //this.FindGameInstallationPath(this.Options.TargetGame);
        }

        #endregion
    }
}

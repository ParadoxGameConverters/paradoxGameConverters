using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Settings;
using Microsoft.Win32;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    /// <summary>
    /// 
    /// </summary>
    public class OpenConverterCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenConverterCommand"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public OpenConverterCommand(ConverterOptions options)
            : base(options)
        {
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected override void OnExecute(object parameter)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.DefaultExt = ".exe";
            dialog.Filter = "EU3toV2Converter.exe | EU3toV2Converter.exe";
            dialog.InitialDirectory = Environment.CurrentDirectory;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                this.Options.Converter = dialog.FileName;
                this.Options.Logger.AddLogEntry(new LogEntry("Converter .exe found at " + this.Options.Converter, LogEntrySeverity.Info, LogEntrySource.UI));
            }
        }
    }
}

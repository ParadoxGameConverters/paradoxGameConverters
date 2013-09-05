using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Settings;
using Microsoft.Win32;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    public class OpenConverterCommand : CommandBase
    {
        public OpenConverterCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override void OnExecute(object parameter)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.DefaultExt = ".exe";
            dialog.Filter = "CK2ToEU3.exe | CK2ToEU3.exe";
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

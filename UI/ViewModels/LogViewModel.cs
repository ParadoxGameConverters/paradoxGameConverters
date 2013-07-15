using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Framework;
using Converter.UI.Settings;
using System.Windows.Input;
using Converter.UI.Commands;
using System.Collections.ObjectModel;

namespace Converter.UI.ViewModels
{
    public class LogViewModel : BaseViewModel, IViewModel
    {
        private ICommand saveCommand;

        private ICommand convertCommand;

        public LogViewModel(IView view, ConverterOptions options)
            : base(view, "Log", options)
        {
        }

        public ICommand SaveCommand
        {
            get
            {
                return this.saveCommand ?? (this.saveCommand = new SaveCommand(this.Options));
            }
        }

        public ICommand ConvertCommand
        {
            get
            {
                return this.convertCommand ?? (this.convertCommand = new ConvertCommand(this.Options));
            }
        }

        public ReadOnlyObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.Options.Logger.LogEntries;
            }
        }
    }
}

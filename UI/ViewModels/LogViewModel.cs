using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Framework;
using Converter.UI.Settings;
using System.Windows.Input;
using Converter.UI.Commands;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using Converter.UI.Views;

namespace Converter.UI.ViewModels
{
    public class LogViewModel : BaseViewModel, IViewModel
    {
        private ICommand saveCommand;

        private ICommand convertCommand;

        public LogViewModel(IView view, ConverterOptions options)
            : base(view, "Convert", options)
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

        protected override void OnTabSelected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            // Hack to monitor the LogEntries collection for collectionchange events to facilitate auto-scrolling in the datagrid. Yeah, it's ugly.
            var logEntries = this.LogEntries as INotifyCollectionChanged;
            logEntries.CollectionChanged += logEntries_CollectionChanged;
        }

        protected override void OnTabDeselected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            var logEntries = this.LogEntries as INotifyCollectionChanged;
            logEntries.CollectionChanged -= logEntries_CollectionChanged;
        }

        private void logEntries_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            var logView = this.View as LogView;
            logView.ScrollDataGridToEnd();
        }
    }
}

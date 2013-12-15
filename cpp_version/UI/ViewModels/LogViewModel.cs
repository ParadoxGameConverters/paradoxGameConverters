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
    /// <summary>
    /// The logviewmodel is the viewmodel for the view that displays the log, as well as the save and convert buttons.
    /// </summary>
    public class LogViewModel : BaseViewModel, IViewModel
    {
        private ICommand saveCommand;

        private ICommand convertCommand;

        /// <summary>
        /// Initializes a new instance of the <see cref="LogViewModel"/> class.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="options">The options.</param>
        public LogViewModel(IView view, ConverterOptions options)
            : base(view, "Convert", options)
        {
        }

        /// <summary>
        /// Gets the save command.
        /// </summary>
        /// <value>
        /// The save command.
        /// </value>
        public ICommand SaveCommand
        {
            get
            {
                return this.saveCommand ?? (this.saveCommand = new SaveCommand(this.Options));
            }
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
                return this.convertCommand ?? (this.convertCommand = new ConvertCommand(this.Options));
            }
        }

        /// <summary>
        /// Gets the log entries.
        /// </summary>
        /// <value>
        /// The log entries.
        /// </value>
        public ReadOnlyObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.Options.Logger.LogEntries;
            }
        }

        /// <summary>
        /// Called when the log tab is selected
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.Controls.SelectionChangedEventArgs"/> instance containing the event data.</param>
        protected override void OnTabSelected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            // Hack to monitor the LogEntries collection for collectionchange events to facilitate auto-scrolling in the datagrid. Yeah, it's ugly.
            var logEntries = this.LogEntries as INotifyCollectionChanged;
            logEntries.CollectionChanged += logEntries_CollectionChanged;
        }

        /// <summary>
        /// Called when this tab is deselected
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.Controls.SelectionChangedEventArgs"/> instance containing the event data.</param>
        protected override void OnTabDeselected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            var logEntries = this.LogEntries as INotifyCollectionChanged;
            logEntries.CollectionChanged -= logEntries_CollectionChanged;
        }

        /// <summary>
        /// Handles the CollectionChanged event of the logEntries control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="NotifyCollectionChangedEventArgs"/> instance containing the event data.</param>
        private void logEntries_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            var logView = this.View as LogView;
            logView.ScrollDataGridToEnd();
        }
    }
}

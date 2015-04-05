using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Common;
using Frontend.Core.Logging;
using Frontend.Core.ViewModels.Interfaces;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Threading;
using System.Windows;
using System.Windows.Threading;

namespace Frontend.Core.ViewModels
{
    public class LogViewModel : ViewModelBase, ILogViewModel
    {
        private OpenUriCommand openUriCommand;

        private ObservableCollection<LogEntry> logEntries;

        public LogViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
            this.EventAggregator.Subscribe(this);
        }

        public OpenUriCommand OpenUriCommand
        {
            get
            {
                return this.openUriCommand ?? (this.openUriCommand = new OpenUriCommand(this.EventAggregator));
            }
        }

        protected override void OnLoading(object parameter)
        {
            this.EventAggregator.Subscribe(this);
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);

            this.LogEntries.CollectionChanged += this.LogEntries_CollectionChanged;
        }
        
        public ObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.logEntries ?? (this.logEntries = new ObservableCollection<LogEntry>());
            }
        }

        public void Handle(LogEntry message)
        {
            this.MarshallMethod(() => this.LogEntries.Add(message), DispatcherPriority.Send);
        }
        
        private void LogEntries_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
        }

        /// <summary>
        /// Marshalls the method.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="priority">The priority.</param>
        private void MarshallMethod(System.Action action, DispatcherPriority priority)
        {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Application.Current.Dispatcher.Invoke(action, priority);
                return;
            }

            action();
        }
    }
}

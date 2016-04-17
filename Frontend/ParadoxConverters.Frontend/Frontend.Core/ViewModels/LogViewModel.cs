using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Windows;
using System.Windows.Threading;
using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Common;
using Frontend.Core.Logging;
using Frontend.Core.ViewModels.Interfaces;
using Action = System.Action;

namespace Frontend.Core.ViewModels
{
    public class LogViewModel : ViewModelBase, ILogViewModel
    {
        private ObservableCollection<LogEntry> logEntries;
        private OpenUriCommand openUriCommand;

        public LogViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
            EventAggregator.Subscribe(this);
        }

        public OpenUriCommand OpenUriCommand
        {
            get { return openUriCommand ?? (openUriCommand = new OpenUriCommand(EventAggregator)); }
        }

        public ObservableCollection<LogEntry> LogEntries
        {
            get { return logEntries ?? (logEntries = new ObservableCollection<LogEntry>()); }
        }

        public void Handle(LogEntry message)
        {
            MarshallMethod(() => LogEntries.Add(message), DispatcherPriority.Send);
        }

        protected override void OnLoading(object parameter)
        {
            EventAggregator.Subscribe(this);
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);

            LogEntries.CollectionChanged += LogEntries_CollectionChanged;
        }

        private void LogEntries_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
        }

        /// <summary>
        ///     Marshalls the method.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="priority">The priority.</param>
        private void MarshallMethod(Action action, DispatcherPriority priority)
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
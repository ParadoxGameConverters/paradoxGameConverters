using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Threading;
using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.ViewModels;
using Frontend.Core.ViewModels.Interfaces;

namespace Frontend.Client
{
    public class AppBootstrapper : BootstrapperBase
    {
        private SimpleContainer container;
        private IEventAggregator eventAggregator;

        public AppBootstrapper()
        {
            StartRuntime();
        }

        protected IEventAggregator EventAggregator
        {
            get { return eventAggregator ?? (eventAggregator = container.GetInstance<IEventAggregator>()); }
        }

        protected override void Configure()
        {
            container = new SimpleContainer();
            container.Singleton<IWindowManager, WindowManager>();
            container.Singleton<IEventAggregator, EventAggregator>();
            container.PerRequest<IShell, ShellViewModel>();
            container.PerRequest<IFrameViewModel, FrameViewModel>();
            container.PerRequest<IWelcomeViewModel, WelcomeViewModel>();
        }

        protected override object GetInstance(Type service, string key)
        {
            var instance = container.GetInstance(service, key);
            if (instance != null)
            {
                return instance;
            }

            throw new InvalidOperationException("Could not locate any instances.");
        }

        protected override IEnumerable<Assembly> SelectAssemblies()
        {
            var assemblies = base.SelectAssemblies().ToList();

            // This includes the Frontend.Core assembly.
            assemblies.Add(Assembly.GetAssembly(typeof (FrameViewModel)));

            return assemblies;
        }

        protected override IEnumerable<object> GetAllInstances(Type service)
        {
            return container.GetAllInstances(service);
        }

        protected override void BuildUp(object instance)
        {
            container.BuildUp(instance);
        }

        protected override void OnStartup(object sender, StartupEventArgs e)
        {
            DisplayRootViewFor<IShell>();

            var logEntry = new LogEntry("Startup complete: Application was started from", LogEntrySeverity.Info,
                LogEntrySource.UI, Environment.CurrentDirectory);

            EventAggregator.PublishOnUIThread(logEntry);
        }

        protected override void OnUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            EventAggregator.PublishOnUIThread(new LogEntry("Error occured: " + e.Exception.Message,
                LogEntrySeverity.Error, LogEntrySource.UI));
        }
    }
}
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using Caliburn.Micro;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Events.EventArgs;
using Frontend.Core.Factories;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Navigation;
using Frontend.Core.ViewModels.Interfaces;

namespace Frontend.Core.ViewModels
{
    public class WelcomeViewModel : StepViewModelBase, IWelcomeViewModel
    {
        private ConverterSettingsFactory settingsFactory;
        private ObservableCollection<ConverterSettings> supportedConverters;

        public WelcomeViewModel(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        protected ConverterSettingsFactory SettingsFactory
        {
            get { return settingsFactory ?? (settingsFactory = new ConverterSettingsFactory(EventAggregator)); }
        }

        public ObservableCollection<ConverterSettings> SupportedConverters
        {
            get
            {
                if (supportedConverters == null)
                {
                    supportedConverters =
                        SettingsFactory.BuildModels<ConverterSettings>(Path.Combine(Environment.CurrentDirectory,
                            "Configuration/SupportedConvertersDefault.xml"));

                    supportedConverters.First().IsSelected = true;
                }

                return supportedConverters;
            }
        }

        public void Handle(IConverterSettings message)
        {
            Options.CurrentConverter = message;

            // A new converter has been set. This means we can add the rest of the view models for that converter to the list of steps
            if (Options.CurrentConverter.Categories != null)
            {
                IList<IStep> steps = new List<IStep>();

                foreach (var category in Options.CurrentConverter.Categories)
                {
                    steps.Add(new PreferencesViewModel(EventAggregator, Options, category));
                }

                steps.Add(new ConvertViewModel(EventAggregator, Options, new OperationProvider(EventAggregator)));

                EventAggregator.PublishOnUIThread(new PreferenceStepOperationArgs(PreferenceOperation.Clear, null));
                EventAggregator.PublishOnUIThread(new PreferenceStepOperationArgs(PreferenceOperation.AddSteps, steps));
            }
        }

        /// <summary>
        ///     Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool IsValid
        {
            get { return true; }
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);
            EventAggregator.Subscribe(this);
        }
    }
}
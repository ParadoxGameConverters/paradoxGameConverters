using Caliburn.Micro;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Events.EventArgs;
using Frontend.Core.Factories;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Navigation;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;

namespace Frontend.Core.ViewModels
{
    public class WelcomeViewModel : StepViewModelBase, IWelcomeViewModel
    {
        private ObservableCollection<ConverterSettings> supportedConverters;
        private ConverterSettingsFactory settingsFactory;

        public WelcomeViewModel(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        protected ConverterSettingsFactory SettingsFactory
        {
            get
            {
                return this.settingsFactory ?? (this.settingsFactory = new ConverterSettingsFactory(this.EventAggregator));
            }
        }

        public ObservableCollection<ConverterSettings> SupportedConverters
        {
            get
            {
                if (this.supportedConverters == null)
                {
                    this.supportedConverters =
                        this.SettingsFactory.BuildModels<ConverterSettings>(Path.Combine(Environment.CurrentDirectory,
                            "Configuration/SupportedConvertersDefault.xml"));

                    this.supportedConverters.First().IsSelected = true;
                }

                return this.supportedConverters;
            }
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);
            this.EventAggregator.Subscribe(this);
        }

        public void Handle(IConverterSettings message)
        {
            this.Options.CurrentConverter = message;

            // A new converter has been set. This means we can add the rest of the view models for that converter to the list of steps
            if (this.Options.CurrentConverter.Categories != null)
            {
                IList<IStep> steps = new List<IStep>();

                foreach (var category in this.Options.CurrentConverter.Categories)
                {
                    steps.Add(new PreferencesViewModel(this.EventAggregator, this.Options, category));
                }

                steps.Add(new ConvertViewModel(this.EventAggregator, this.Options, new OperationProvider(this.EventAggregator)));

                this.EventAggregator.PublishOnUIThread(new PreferenceStepOperationArgs(PreferenceOperation.Clear, null));
                this.EventAggregator.PublishOnUIThread(new PreferenceStepOperationArgs(PreferenceOperation.AddSteps, steps));
            }
        }

        /// <summary>
        /// Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool IsValid
        {
            get
            {
                return true;
            }
        }
    }
}

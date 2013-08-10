using System.Text;
using System.Windows.Input;
using Converter.UI.Framework;
using Converter.UI.Settings;

namespace Converter.UI.ViewModels
{
    public class SummaryViewModel : BaseViewModel, IViewModel
    {
        private string summary;

        public SummaryViewModel(IView view, ConverterOptions options)
            : base(view, "Summary", options)
        {
            this.BuiltOutputString();
        }

        public string Summary
        {
            get
            {
                return this.summary;
            }

            set
            {
                if (this.summary == value)
                {
                    return;
                }

                this.summary = value;
                this.RaisePropertyChanged("Summary");
            }
        }

        public ICommand SaveCommand { get; set; }

        public ICommand ConvertCommand { get; set; }

        protected override void OnTabSelected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            this.BuiltOutputString();
        }
        
        private void BuiltOutputString()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("configuration =");
            sb.AppendLine("{");

            // Output source and target game settings
            sb.AppendLine("# Installation paths:");
            sb.AppendLine(this.Options.SourceGame.ConfigurationFileDirectoryTagName + " = " + this.Options.SourceGame.InstallationPath);
            sb.AppendLine(this.Options.TargetGame.ConfigurationFileDirectoryTagName + " = " + this.Options.TargetGame.InstallationPath);

            // Preferences
            foreach (PreferenceCategory category in this.Options.ConfigurationProvider.PreferenceCategories)
            {
                sb.AppendLine();
                sb.AppendLine("# " + category.FriendlyName);
                sb.AppendLine();

                foreach (IPreference preference in category.Preferences)
                {
                    sb.AppendLine(" # " + preference.FriendlyName + ": " + preference.Description);

                    var preferenceAsDouble = preference as Preference<double>;
                    var preferenceAsString = preference as Preference<string>;

                    if (preferenceAsDouble != null)
                    {
                        var userChoice = preferenceAsDouble.UserChoice as NumericPreferenceEntry;
                        sb.AppendLine(preferenceAsDouble.Name + " = " + userChoice.Value);
                    }
                    else if (preferenceAsString != null)
                    {
                        sb.AppendLine(preferenceAsString.Name + " = " + preferenceAsString.UserChoice.Name);
                    }
                }
            }

            sb.AppendLine("}");
            this.Summary = sb.ToString();
        }
    }
}

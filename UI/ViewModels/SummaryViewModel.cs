using System.Text;
using System.Windows.Input;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Converter.UI.Commands;
using System.Linq;

namespace Converter.UI.ViewModels
{
    public class SummaryViewModel : BaseViewModel, IViewModel
    {
        private string summary;

        private ICommand saveCommand;

        private ICommand convertCommand;

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

        public ICommand SaveCommand
        {
            get
            {
                return this.saveCommand ?? (this.saveCommand = new SaveCommand(this.Options, this.Summary));
            }
        }

        public ICommand ConvertCommand
        {
            get
            {
                return this.convertCommand ?? (this.convertCommand = new ConvertCommand(this.Options));
            }
        }

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
            sb.AppendLine(this.Options.SourceGame.ConfigurationFileDirectoryTagName + " = \"" + this.Options.SourceGame.InstallationPath + "\"");
            sb.AppendLine(this.Options.TargetGame.ConfigurationFileDirectoryTagName + " = \"" + this.Options.TargetGame.InstallationPath + "\"");
            
            sb.AppendLine();
            sb.AppendLine("# Use converter mod: ");
            sb.AppendLine("useConverterMod = " + "\"" + (this.Options.UseConverterMod ? "yes" : "no") + "\"");

            // Preferences
            foreach (PreferenceCategory category in this.Options.ConfigurationProvider.PreferenceCategories)
            {
                sb.AppendLine();
                sb.AppendLine("# " + category.FriendlyName);
                sb.AppendLine();

                category.Preferences.ForEach(p => sb.AppendLine(p.ToString()));
            }

            sb.AppendLine("}");
            this.Summary = sb.ToString();
        }
    }
}

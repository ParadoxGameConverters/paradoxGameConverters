using System.IO;
using Converter.UI.Settings;

namespace Converter.UI.Commands
{
    public class ConvertCommand : CommandBase
    {
        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            return !string.IsNullOrEmpty(this.Options.SourceSaveGame);
        }

        protected override void OnExecute(object parameter)
        {
            if (!File.Exists(this.Options.SourceSaveGame))
            {
                return;
            } 
        }
    }
}

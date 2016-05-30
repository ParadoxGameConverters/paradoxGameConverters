using System.Linq;
using System.Text;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Helpers
{
    public class OutputConfigurationFileHelper : IOutputConfigurationFileHelper
    {
        private readonly IEnvironmentProxy environmentProxy;
        private readonly IFileProxy fileProxy;

        public OutputConfigurationFileHelper(IFileProxy fileProxy, IEnvironmentProxy environmentProxy)
        {
            this.fileProxy = fileProxy;
            this.environmentProxy = environmentProxy;
        }

        /// <summary>
        ///     Constructs the string that will be saved to disk as the config file.
        /// </summary>
        /// <returns></returns>
        public string BuiltOutputString(IConverterSettings converterSettings, IDirectoryHelper directoryHelper)
        {
            var sb = new StringBuilder();

            sb.AppendLine(ReadTextFile(environmentProxy.GetFrontendWorkingDirectory() + "\\Docs\\license.txt"));

            sb.AppendLine("configuration =");
            sb.AppendLine("{");

            // TODO: Mod support
            // Output source and target game settings

            // Why the "where clause": Some entries in the requireditems list is only used by the frontend. 
            // Those that needs to go into configuration.txt has the "TagName" attribute check, the others don't.
            foreach (
                var folder in
                    converterSettings.RequiredItems.OfType<IRequiredFolder>()
                        .Where(f => !string.IsNullOrEmpty(f.TagName)))
            {
                sb.AppendLine("\t# " + folder.FriendlyName + ": " + folder.Description);
                sb.AppendLine("\t" + folder.TagName + " = \"" + folder.SelectedValue + "\"");
                sb.AppendLine();
            }

            // Preferences
            foreach (var category in converterSettings.Categories)
            {
                //sb.AppendLine();
                //sb.AppendLine("\t# " + category.FriendlyName);
                //sb.AppendLine();

                category.Preferences.ForEach(p => sb.AppendLine("\t" + p.ToString()));
            }

            sb.AppendLine("}");
            return sb.ToString();
        }

        public string ReadTextFile(string path)
        {
            if (!fileProxy.Exists(path))
            {
                return string.Empty;
            }

            var text = fileProxy.ReadAllText(path);

            return text;
        }
    }
}
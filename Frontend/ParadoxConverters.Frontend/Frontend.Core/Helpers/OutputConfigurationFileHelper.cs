using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Helpers
{
    public static class OutputConfigurationFileHelper
    {
        /// <summary>
        /// Constructs the string that will be saved to disk as the config file.
        /// </summary>
        /// <returns></returns>
        public static string BuiltOutputString(IConverterSettings converterSettings)
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("configuration =");
            sb.AppendLine("{");

            // TODO: Mod support
            // Output source and target game settings
            sb.AppendLine("\t# Installation and mod folder paths:");

            foreach (IRequiredFolder folder in converterSettings.RequiredItems.OfType<IRequiredFolder>())
            {
                sb.AppendLine("\t" + folder.TagName + " = \"" + folder.SelectedValue + "\"");
            }

            //var games = new List<IGameConfiguration>() { converterSettings.SourceGame, converterSettings.TargetGame };            

            //foreach (var game in games)
            //{
            //    sb.AppendLine("\t" + game.InstallationDirectoryTagName + " = \"" + game.AbsoluteInstallationPath + "\"");

            //    //if (game.IsConfiguredToUseMods)
            //    //{
            //    sb.AppendLine();

            //    if (!string.IsNullOrEmpty(game.ModDirectoryTagName))
            //    {
            //        sb.AppendLine("\t" + game.ModDirectoryTagName + " = \"" + game.AbsoluteModPath + "\"");
            //        sb.AppendLine();
            //    }
            //    //}

            //    //if (!string.IsNullOrEmpty(game.TempDirectoryTagName))
            //    //{
            //    //    sb.AppendLine("\t" + game.TempDirectoryTagName + " = \"" + game.AbsoluteTempDirectoryPath + "\"");
            //    //    sb.AppendLine();
            //    //}
            //}

            //if (!string.IsNullOrEmpty(converterSettings.NativeParadoxExportDirectoryTag))
            //{
            //    sb.AppendLine("\t" + converterSettings.NativeParadoxExportDirectoryTag + " = \"" + converterSettings.NativeParadoxExportDirectory + "\"");
            //    sb.AppendLine();
            //}

            //sb.AppendLine();
            //sb.AppendLine("\t# Mod Options: ");

            //foreach (var game in games.FindAll(g => g.IsConfiguredToUseMods))
            //{
            //    sb.AppendLine();
            //    sb.AppendLine("\t# " + game.CurrentModTagName + ": the mod used while playing the source game");
            //    sb.AppendLine("\t" + game.CurrentModTagName + " = \"" + (!game.CurrentMod.IsDummyItem ? game.CurrentMod.Name : string.Empty) + "\"");
            //    sb.AppendLine();
            //}

            sb.AppendLine("\tuseConverterMod = " + "\"" + (converterSettings.UseConverterMod ? "yes" : "no") + "\"");

            // Preferences
            foreach (IPreferenceCategory category in converterSettings.Categories)
            {
                sb.AppendLine();
                sb.AppendLine("\t# " + category.FriendlyName);
                sb.AppendLine();

                category.Preferences.ForEach(p => sb.AppendLine("\t" + p.ToString()));
            }

            sb.AppendLine("}");
            return sb.ToString();
        }
    }
}

using System.Linq;
using Converter.UI.Framework;
using Converter.UI.Providers;
using System.Text;
using System.Collections.ObjectModel;

namespace Converter.UI.Settings
{
    public class ConverterOptions : NotifiableBase
    {
        private GameConfiguration sourceGame;
        private GameConfiguration targetGame;
        private string sourceSaveGame;
        private string converter;
        private string outputConfiguration;
        private ConfigurationProvider configurationProvider;
        private bool useConverterMod;
        private Logger logger;

        public ConverterOptions()
        {
            this.SourceGame = this.ConfigurationProvider.SourceGames.First();
            this.TargetGame = this.ConfigurationProvider.TargetGames.First();
        }

        public GameConfiguration SourceGame
        {
            get
            {
                return this.sourceGame;
            }

            set
            {
                if (this.sourceGame == value)
                {
                    return;
                }

                this.sourceGame = value;
                this.RaisePropertyChanged("SourceGame");
            }
        }

        public GameConfiguration TargetGame
        {
            get
            {
                return this.targetGame;
            }

            set
            {
                if (this.targetGame == value)
                {
                    return;
                }

                this.targetGame = value;
                this.RaisePropertyChanged("TargetGame");
            }
        }

        public string Converter
        {
            get
            {
                return this.converter;
            }

            set
            {
                if (this.converter == value)
                {
                    return;
                }

                this.converter = value;
                this.RaisePropertyChanged("Converter");
            }
        }

        public bool UseConverterMod
        {
            get
            {
                return this.useConverterMod;
            }

            set
            {
                if (this.useConverterMod == value)
                {
                    return;
                }

                this.useConverterMod = value;
                this.RaisePropertyChanged("UseConverterMod");
            }
        }

        public string SourceSaveGame
        {
            get
            {
                return this.sourceSaveGame;
            }

            set
            {
                if (this.sourceSaveGame == value)
                {
                    return;
                }

                this.sourceSaveGame = value;
                this.RaisePropertyChanged("SourceSaveGame");
            }
        }

        public ConfigurationProvider ConfigurationProvider
        {
            get
            {
                return this.configurationProvider ?? (this.configurationProvider = new ConfigurationProvider());
            }
        }

        public Logger Logger
        {
            get
            {
                return this.logger ?? (this.logger = new Logger());
            }
        }

        public string OutputConfiguration
        {
            get
            {
                return this.outputConfiguration ?? (this.outputConfiguration = this.BuiltOutputString());
            }

            //set
            //{
            //    if (this.outputConfiguration == value)
            //    {
            //        return;
            //    }

            //    this.outputConfiguration = value;
            //    this.RaisePropertyChanged("OutputConfiguration");
            //}
        }
        
        private string BuiltOutputString()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("configuration =");
            sb.AppendLine("{");

            // Output source and target game settings
            sb.AppendLine("\t# Installation paths:");
            sb.AppendLine("\t" + this.SourceGame.ConfigurationFileDirectoryTagName + " = \"" + this.SourceGame.InstallationPath + "\"");
            sb.AppendLine("\t" + this.TargetGame.ConfigurationFileDirectoryTagName + " = \"" + this.TargetGame.InstallationPath + "\"");
            
            sb.AppendLine();
            sb.AppendLine("\t# Use converter mod: ");
            sb.AppendLine("\tuseConverterMod = " + "\"" + (this.UseConverterMod ? "yes" : "no") + "\"");

            // Preferences
            foreach (PreferenceCategory category in this.ConfigurationProvider.PreferenceCategories)
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

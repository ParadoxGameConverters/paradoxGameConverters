using System.Linq;
using Converter.UI.Framework;
using Converter.UI.Providers;
using System.Text;
using System.Collections.ObjectModel;
using System.Collections.Generic;

namespace Converter.UI.Settings
{
    /// <summary>
    /// This class is in some ways the core of the converter UI. Every choice the user makes is stored here, 
    /// and this object is passed along to most viewmodels/commands for various types of interactions.
    /// </summary>
    public class ConverterOptions : NotifiableBase
    {
        private GameConfiguration sourceGame;
        private GameConfiguration targetGame;
        private string sourceSaveGame;
        private string converter;
        private string outputConfiguration;
        private ConfigurationProvider configurationProvider;
        private ModFilesProvider modFilesProvider;
        private bool useConverterMod;
        private Logger logger;

        /// <summary>
        /// Initializes a new instance of the <see cref="ConverterOptions"/> class.
        /// </summary>
        public ConverterOptions()
        {
            // Assumption: Only one source and target game.
            this.SourceGame = this.ConfigurationProvider.SourceGames.First();
            this.TargetGame = this.ConfigurationProvider.TargetGames.First();
        }

        /// <summary>
        /// Gets or sets a value indicating whether conversion was successful.
        /// </summary>
        /// <value>
        /// <c>true</c> if conversion was successful; otherwise, <c>false</c>.
        /// </value>
        public bool WasConversionSuccessful { get; set; }

        /// <summary>
        /// Gets or sets the source game. This is the game the converter converts -from-. 
        /// </summary>
        /// <value>
        /// The source game.
        /// </value>
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

        /// <summary>
        /// Gets or sets the target game. This is the game the converter converts -to-.
        /// </summary>
        /// <value>
        /// The target game.
        /// </value>
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

        /// <summary>
        /// Gets or sets the converter path.
        /// </summary>
        /// <value>
        /// The converter.
        /// </value>
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

        /// <summary>
        /// Gets or sets a value indicating whether to use the converter mod.
        /// </summary>
        /// <value>
        ///   <c>true</c> if [use converter mod]; otherwise, <c>false</c>.
        /// </value>
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

        /// <summary>
        /// Gets or sets the source save game.
        /// </summary>
        /// <value>
        /// The source save game.
        /// </value>
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

        /// <summary>
        /// Gets the configuration provider. The current provider is responsible for turning the configuration.xml file into something sensible.
        /// </summary>
        /// <value>
        /// The configuration provider.
        /// </value>
        public ConfigurationProvider ConfigurationProvider
        {
            get
            {
                return this.configurationProvider ?? (this.configurationProvider = new ConfigurationProvider(this));
            }
        }

        /// <summary>
        /// Gets the logger. 
        /// </summary>
        /// <value>
        /// The logger.
        /// </value>
        public Logger Logger
        {
            get
            {
                return this.logger ?? (this.logger = new Logger());
            }
        }

        /// <summary>
        /// Gets the output configuration. This is essentially all the text that becomes configuration.txt.
        /// </summary>
        /// <value>
        /// The output configuration.
        /// </value>
        public string OutputConfiguration
        {
            get
            {
                return this.outputConfiguration ?? (this.outputConfiguration = this.BuiltOutputString());
            }

            // Permitting the user to change the output configuration while in-memory raises all sorts of issues. Unless explicitly needed, I'm not adding that functionality. 
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

        /// <summary>
        /// Gets the mod files provider. It is responsible for turning the list of mod-related files in configuration.xml into a list of paths for files that needs copying.
        /// </summary>
        /// <value>
        /// The mod files provider.
        /// </value>
        public ModFilesProvider ModFilesProvider
        {
            get
            {
                return this.modFilesProvider ?? (this.modFilesProvider = new ModFilesProvider(this));
            }
        }

        /// <summary>
        /// This resets the output configuration text string. 
        /// <remarks>Essentially a hack in lieu of a more architectural sound way to mark the output configuration as 'dirty'.</remarks>
        /// </summary>
        public void InvalidateOutputConfiguration()
        {
            this.outputConfiguration = null;
        }
        
        /// <summary>
        /// Constructs the string that will be saved to disk as the config file.
        /// </summary>
        /// <returns></returns>
        private string BuiltOutputString()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("configuration =");
            sb.AppendLine("{");

            // Output source and target game settings
            sb.AppendLine("\t# Installation and mod folder paths:");

            var games = new List<GameConfiguration>() { this.SourceGame, this.TargetGame };

            foreach (var game in games)
            {
                sb.AppendLine("\t" + game.ConfigurationFileDirectoryTagName + " = \"" + game.InstallationPath + "\"");

                if (game.IsConfiguredToUseMods)
                {
                    sb.AppendLine();
                    sb.AppendLine("\t" + game.ConfigurationFileModDirectoryTagName + " = \"" + game.ModPath + "\"");
                    sb.AppendLine();
                }
            }
            
            sb.AppendLine();
            sb.AppendLine("\t# Mod Options: ");

            foreach (var game in games.FindAll(g => g.IsConfiguredToUseMods))
            {
                sb.AppendLine();
                sb.AppendLine("\t# " + game.CurrentModTagName + ": the mod used while playing the source game");
                sb.AppendLine("\t" + game.CurrentModTagName + " = \"" + (!game.CurrentMod.IsDummyItem ?  game.CurrentMod.Name : string.Empty) + "\"");
                sb.AppendLine();
            }

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

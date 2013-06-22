using System.Linq;
using Converter.UI.Framework;
using Converter.UI.Providers;

namespace Converter.UI.Settings
{
    public class ConverterOptions : NotifiableBase
    {
        private GameConfiguration sourceGame;
        private GameConfiguration targetGame;
        private string sourceSaveGame;
        private string converter;
        private ConfigurationProvider configurationProvider;
        private bool useConverterMod;

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
    }
}

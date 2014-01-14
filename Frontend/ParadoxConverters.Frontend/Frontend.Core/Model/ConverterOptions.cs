using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model
{
    public class ConverterOptions : PropertyChangedBase, IConverterOptions
    {
        private IGameConfiguration sourceGame;
        private IGameConfiguration targetGame;
        private string absoluteConverterPath;
        private string absoluteSourceSaveGamePath;
        private bool useConverterMod;
        private IConverterSettings currentConverter;

        public ConverterOptions()
        {
        }

        /// <summary>
        /// Gets or sets the current converter settings
        /// </summary>
        public IConverterSettings CurrentConverter
        {
            get
            {
                return this.currentConverter;
            }

            set
            {
                if (this.currentConverter == value)
                {
                    return;
                }

                this.Reset(value);
                this.currentConverter = value;
                this.NotifyOfPropertyChange(() => this.CurrentConverter);
            }
        }

        /// <summary>
        /// Gets or sets the source game. This is the game the converter converts -from-. 
        /// </summary>
        /// <value>
        /// The source game.
        /// </value>
        public IGameConfiguration SourceGame
        {
            get
            {
                return this.sourceGame;
            }
        }

        /// <summary>
        /// Gets or sets the target game. This is the game the converter converts -to-.
        /// </summary>
        /// <value>
        /// The target game.
        /// </value>
        public IGameConfiguration TargetGame
        {
            get
            {
                return this.targetGame;
            }
        }

        /// <summary>
        /// Gets or sets the converter path.
        /// </summary>
        /// <value>
        /// The converter.
        /// </value>
        public string AbsoluteConverterPath
        {
            get
            {
                return this.absoluteConverterPath;
            }

            set
            {
                if (this.absoluteConverterPath == value)
                {
                    return;
                }

                this.absoluteConverterPath = value;
                this.NotifyOfPropertyChange(() => this.AbsoluteConverterPath);
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
                this.NotifyOfPropertyChange(() => this.UseConverterMod);
            }
        }

        /// <summary>
        /// Gets or sets the source save game.
        /// </summary>
        /// <value>
        /// The source save game.
        /// </value>
        public string AbsoluteSourceSaveGamePath
        {
            get
            {
                return this.absoluteSourceSaveGamePath;
            }

            set
            {
                if (this.absoluteSourceSaveGamePath == value)
                {
                    return;
                }

                this.absoluteSourceSaveGamePath = value;
                this.NotifyOfPropertyChange(() => this.AbsoluteSourceSaveGamePath);
            }
        }

        public void Reset(IConverterSettings newSettings)
        {

        }
    }
}

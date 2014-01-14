using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Interfaces
{
    public interface IConverterOptions
    {
        /// <summary>
        /// Gets or sets the current converter settings
        /// </summary>
        IConverterSettings CurrentConverter { get; set; }
        
        /// <summary>
        /// Gets or sets the source game. This is the game the converter converts -from-. 
        /// </summary>
        /// <value>
        /// The source game.
        /// </value>
        IGameConfiguration SourceGame { get; }

        /// <summary>
        /// Gets or sets the target game. This is the game the converter converts -to-.
        /// </summary>
        /// <value>
        /// The target game.
        /// </value>
        IGameConfiguration TargetGame { get; }

        /// <summary>
        /// Gets or sets the converter path.
        /// </summary>
        /// <value>
        /// The converter.
        /// </value>
        string AbsoluteConverterPath { get; set; }
        
        /// <summary>
        /// Gets or sets a value indicating whether to use the converter mod.
        /// </summary>
        /// <value>
        ///   <c>true</c> if [use converter mod]; otherwise, <c>false</c>.
        /// </value>
        bool UseConverterMod { get; }

        /// <summary>
        /// Gets or sets the source save game.
        /// </summary>
        /// <value>
        /// The source save game.
        /// </value>
        string AbsoluteSourceSaveGamePath { get; set; }
    }
}

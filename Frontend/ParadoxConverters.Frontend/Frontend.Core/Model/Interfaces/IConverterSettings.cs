using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Interfaces
{
    public interface IConverterSettings : IModel
    {
        string Name { get; set; }
        string FriendlyName { get; set; }
        string DefaultConfigurationFile { get; set; }
        string UserConfigurationFile { get; set; }
        string ConverterExeName { get; set; }
        bool IsSelected { get; set; }
        IGameConfiguration SourceGame { get; set; }
        IGameConfiguration TargetGame { get; set; }
        IList<IPreferenceCategory> Categories { get; set; }

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

        //string NativeParadoxExportDirectory { get; set; }

        //string NativeParadoxExportDirectoryTag { get; set; }
    }
}

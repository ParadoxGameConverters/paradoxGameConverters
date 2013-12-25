using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace Converter.UI.Settings
{
    /// <summary>
    /// Interface for each preference. A preference is defined as a single setting in the configuration.xml/configuration.txt file. 
    /// Each setting can have multiple choices, default values and various other properties exposed by this interface.
    /// </summary>
    public interface IPreference
    {
        /// <summary>
        /// Gets or sets the preference name. Must match the name of the preference in configuration.txt
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        string Name { get; set; }

        /// <summary>
        /// Gets or sets the friendly preference name. Usually more readable to humans than the normal name, which tends to lack - for instance - spaces.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        string FriendlyName { get; set; }

        /// <summary>
        /// Gets or sets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        string Description { get; set; }

        /// <summary>
        /// Gets a value indicating whether this preference has a set of predefined choices.
        /// </summary>
        /// <value>
        /// <c>true</c> if yes; otherwise, <c>false</c>.
        /// </value>
        bool HasPreDefinedChoices { get; }

        /// <summary>
        /// Gets or sets a value indicating whether this preference has a directly editable value (as opposed to pre-defined choices, for instance)
        /// </summary>
        /// <value>
        /// <c>true</c> if yes; otherwise, <c>false</c>.
        /// </value>
        bool HasDirectlyEditableValue { get; set; }

        /// <summary>
        /// Gets a value indicating whether this preference contains a numeric value
        /// </summary>
        /// <value>
        ///   <c>true</c> if yes, otherwise, <c>false</c>.
        /// </value>
        bool IsNumeric { get; }
        
        /// <summary>
        /// Gets the list of pre-defined IPreferenceEntry objects. These are the pre-defined user choices, if not null.
        /// </summary>
        /// <value>
        /// The entries.
        /// </value>
        IList<IPreferenceEntry> Entries { get; }

        /// <summary>
        /// Gets or sets the minimum value.
        /// </summary>
        /// <value>
        /// The minimum value.
        /// </value>
        double MinValue { get; set; }

        /// <summary>
        /// Gets or sets the maximum value.
        /// </summary>
        /// <value>
        /// The maximum value.
        /// </value>
        double MaxValue { get; set; }

        /// <summary>
        /// Gets or sets the current value.
        /// </summary>
        /// <value>
        /// The value.
        /// </value>
        string Value { get; set; }

        /// <summary>
        /// Gets or sets the selected entry. Only relevant if this list has a list of Entries to choose from.
        /// </summary>
        /// <value>
        /// The selected entry.
        /// </value>
        IPreferenceEntry SelectedEntry { get; set; }

        #region [ Date related ]

        /// <summary>
        /// Gets a value indicating whether this preference contains a date value
        /// </summary>
        bool IsDate { get; }

        /// <summary>
        /// Gets or sets the minimum value.
        /// </summary>
        /// <value>
        /// The minimum value.
        /// </value>
        DateTime MinDateValue { get; set; }

        /// <summary>
        /// Gets or sets the maximum value.
        /// </summary>
        /// <value>
        /// The maximum value.
        /// </value>
        DateTime MaxDateValue { get; set; }

        #endregion
    }
}

using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model
{
    /// <summary>
    /// An instance of this class represents a supported mod.
    /// A supported mod would be a mod the source game was played with, such as Ck2++.
    /// </summary>
    public class Mod : PropertyChangedBase, IMod
    {
        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether this object representes a dummy item (that is, not a real mod)
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is dummy item]; otherwise, <c>false</c>.
        /// </value>
        public bool IsDummyItem { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Converter.UI.Settings
{
    public class SupportedMod
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

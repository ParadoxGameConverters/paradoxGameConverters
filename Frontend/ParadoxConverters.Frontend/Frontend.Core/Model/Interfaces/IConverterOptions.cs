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
    }
}

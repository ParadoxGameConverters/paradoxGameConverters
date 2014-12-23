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
        private IConverterSettings currentConverter;

        public ConverterOptions()
        {
        }

        /// <summary>
        /// Gets or sets a value indicating whether conversion was successful.
        /// </summary>
        /// <value>
        /// <c>true</c> if conversion was successful; otherwise, <c>false</c>.
        /// </value>
        public bool WasConversionSuccessful { get; set; }

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

        public void Reset(IConverterSettings newSettings)
        {

        }
    }
}

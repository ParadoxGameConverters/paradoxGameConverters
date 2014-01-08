using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Model
{
    public class ConverterSettings : PropertyChangedBase, IModel
    {
        private bool isSelected;

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string DefaultConfigurationFile { get; set; }
        public string UserConfigurationFile { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether [is selected].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is selected]; otherwise, <c>false</c>.
        /// </value>
        public bool IsSelected
        {
            get
            {
                return this.isSelected;
            }

            set
            {
                if (this.isSelected == value)
                {
                    return;
                }

                this.isSelected = value;
                this.NotifyOfPropertyChange(() => this.IsSelected);
            }
        }
    }
}

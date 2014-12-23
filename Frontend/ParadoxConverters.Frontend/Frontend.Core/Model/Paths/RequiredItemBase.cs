using Caliburn.Micro;
using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    /// <summary>
    /// Base class for required items
    /// </summary>
    public abstract class RequiredItemBase : PropertyChangedBase, IRequiredItemBase
    {
        private string selectedValue;
        private IList<IAlternativePath> alternativePaths;

        protected RequiredItemBase(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string internalTagName)
        {
            this.TagName = tagName;
            this.InternalTagName = internalTagName;
            this.FriendlyName = friendlyName;
            this.Description = description;            
            this.alternativePaths = alternatives;

            // Basically, take the first alternative path that actually exists, and set that as the default value.
            var defaultPath = alternatives.FirstOrDefault(a => a.Exists);          
            if (defaultPath != null)
            {
                this.DefaultValue = defaultPath.Path;
                this.selectedValue = this.DefaultValue;
            }            
        }

        public IList<IAlternativePath> AlternativePaths
        {
            get
            {
                return this.alternativePaths;
            }
        }

        public string FriendlyName { get; private set; }

        public string Description { get; private set; }

        public string TagName { get; private set; }

        public string InternalTagName { get; private set; }

        public string DefaultValue { get; private set; }

        public string SelectedValue
        {
            get
            {
                return this.selectedValue;
            }

            set
            {
                if (this.selectedValue == value)
                {
                    return;
                }

                this.selectedValue = value;
                this.NotifyOfPropertyChange(() => this.SelectedValue);
            }
        }

        public override string ToString()
        {
            return this.TagName + ": Default: " + this.DefaultValue + " - Actual: " + this.SelectedValue;
        }
    }
}

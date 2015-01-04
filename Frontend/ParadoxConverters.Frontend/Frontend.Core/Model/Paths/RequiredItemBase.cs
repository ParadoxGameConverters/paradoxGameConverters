using Caliburn.Micro;
using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    /// <summary>
    /// Base class for required items
    /// </summary>
    public abstract class RequiredItemBase : PropertyChangedBase, IRequiredItemBase, IDataErrorInfo
    {
        private string selectedValue;
        private IList<IAlternativePath> alternativePaths;

        protected RequiredItemBase(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string internalTagName, bool isMandatory)
        {
            this.TagName = tagName;
            this.InternalTagName = internalTagName;
            this.FriendlyName = friendlyName;
            this.Description = description;            
            this.alternativePaths = alternatives;
            this.IsMandatory = isMandatory;

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

        public bool IsMandatory { get; private set; }

        public string FriendlyName { get; private set; }

        public string Description { get; private set; }

        public string TagName { get; private set; }

        public string InternalTagName { get; private set; }

        public string DefaultValue { get; private set; }

        public bool IsValid
        {
            get
            {
                var errors = this["SelectedValue"];
                return errors == null;
            }
        }

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

        #region IDataErrorInfo Members

        public string Error
        {
            get { throw new NotImplementedException(); }
        }

        public string this[string columnName]
        {
            get
            {
                string result = null;
                if (columnName == "SelectedValue")
                {
                    result = this.ValidateSelectedValue();
                }
                return result;
            }
        }

        #endregion

        public abstract string ValidateSelectedValue();

        public override string ToString()
        {
            return this.TagName + ": Default: " + this.DefaultValue + " - Actual: " + this.SelectedValue + " - IsValid: " + this.IsValid;
        }
    }
}

using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using Caliburn.Micro;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Model.Paths
{
    /// <summary>
    ///     Base class for required items
    /// </summary>
    public abstract class RequiredItemBase : PropertyChangedBase, IRequiredItemBase, IDataErrorInfo
    {
        private string _selectedValue;

        protected RequiredItemBase(string tagName, string friendlyName, string description,
            IList<IAlternativePath> alternatives, string internalTagName, bool isMandatory, bool isHidden)
        {
            TagName = tagName;
            InternalTagName = internalTagName;
            FriendlyName = friendlyName;
            Description = description;
            AlternativePaths = alternatives;
            IsMandatory = isMandatory;
            IsHidden = isHidden;

            // Basically, take the first alternative path that actually exists, and set that as the default value.
            var defaultPath = alternatives.FirstOrDefault(a => a.Exists);
            if (defaultPath != null)
            {
                DefaultValue = defaultPath.Path;
                _selectedValue = DefaultValue;
            }
        }

        public IList<IAlternativePath> AlternativePaths { get; private set; }
        public bool IsMandatory { get; private set; }
        public bool IsHidden { get; private set; }
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
            get { return _selectedValue; }

            set
            {
                if (_selectedValue == value)
                {
                    return;
                }

                _selectedValue = value;
                NotifyOfPropertyChange(() => SelectedValue);
            }
        }

        public abstract string ValidateSelectedValue();

        public override string ToString()
        {
            return TagName + ": Default: " + DefaultValue + " - Actual: " + SelectedValue + " - IsValid: " + IsValid;
        }

        #region IDataErrorInfo Members

        public string Error
        {
            get { return null; }
        }

        public string this[string columnName]
        {
            get
            {
                string result = null;
                if (columnName == "SelectedValue")
                {
                    result = ValidateSelectedValue();
                }
                return result;
            }
        }

        #endregion
    }
}
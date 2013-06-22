using System.ComponentModel;
using Converter.UI.Enums;
using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    public abstract class PreferenceEntry<T> : NotifiableBase, IPreferenceEntry, IDataErrorInfo
    {
        protected PreferenceEntry(string name, string friendlyName, string description, PreferenceType type, IPreference parent)
        {
            this.Name = name;
            this.Description = description;
            this.Type = type;
            this.FriendlyName = friendlyName;
            this.Parent = parent;
        }

        public string Name { get; private set; }
        public string FriendlyName { get; private set; }
        public string Description { get; private set; }
        public PreferenceType Type { get; private set; }
        public IPreference Parent { get; private set; }
        
        public string Error
        {
            get
            {
                return null;
            }
        }

        public string this[string name]
        {
            get
            {
                string result = this.OnValidateProperty(name);

                return result;
            }
        }

        protected virtual string OnValidateProperty(string propertyName)
        {
            return null;
        }

        protected void UpdateParentUserChoice()
        {
            Preference<T> parent = this.Parent as Preference<T>;
            parent.UserChoice = this;
        }
    }
}

using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.PreferenceEntries;
using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences;
using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class PreferenceCategoryFactory : FactoryBase
    {
        public PreferenceCategoryFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "category")
        {
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            var category = new PreferenceCategory();
            category.FriendlyName = XElementHelper.ReadStringValue(element, "friendlyName");

            // Read the list of Preference tags
            var foundPreferences = XElementHelper.ReadEnumerable(element, "preference");

            // For each preference Tag, store the values into a new Preference object
            foreach (var foundPreference in foundPreferences)
            {
                // Ugly way of reading shared properties without duplicating code. 
                Action<IPreference> readSharedProperties = (preference) =>
                {
                    preference.Name = XElementHelper.ReadStringValue(foundPreference, "name");
                    preference.FriendlyName = XElementHelper.ReadStringValue(foundPreference, "friendlyName");
                    preference.Description = XElementHelper.ReadStringValue(foundPreference, "description", false);

                    preference.HasDirectlyEditableValue = XElementHelper.ReadBoolValue(foundPreference, "hasDirectlyEditableValue", false);

                    category.Preferences.Add(preference);

                };

                // Determine preference type
                var isDate = XElementHelper.ReadBoolValue(foundPreference, "isDate", false);
                var isNumeric = XElementHelper.ReadBoolValue(foundPreference, "isNumeric", false);

                if (isDate)
                {
                    var preference = new DatePreference();

                    preference.DateFormat = XElementHelper.ReadStringValue(foundPreference, "dateFormat");
                    preference.MinValue = XElementHelper.ReadDateValue(foundPreference, "minDateValue", preference.DateFormat, false);
                    preference.MaxValue = XElementHelper.ReadDateValue(foundPreference, "maxDateValue", preference.DateFormat, false);
                    preference.Value = XElementHelper.ReadDateValue(foundPreference, "value", preference.DateFormat, true);

                    // Read the list of entryOption tags
                    var foundEntries = XElementHelper.ReadEnumerable(foundPreference, "entryOption", false);

                    // For each tag, read the values into a new PreferenceEntry object
                    foreach (var entry in foundEntries)
                    {
                        preference.Entries.Add(this.BuildPreferenceEntry<IDatePreferenceEntry>(preference, entry));
                    }

                    readSharedProperties(preference);
                }
                else if (isNumeric)
                {
                    var preference = new NumericPreference();

                    preference.MinValue = XElementHelper.ReadDoubleValue(foundPreference, "minValue", false);
                    preference.MaxValue = XElementHelper.ReadDoubleValue(foundPreference, "maxValue", false);
                    preference.Value = XElementHelper.ReadDoubleValue(foundPreference, "value", true);

                    // Read the list of entryOption tags
                    var foundEntries = XElementHelper.ReadEnumerable(foundPreference, "entryOption", false);

                    // For each tag, read the values into a new PreferenceEntry object
                    foreach (var entry in foundEntries)
                    {
                        preference.Entries.Add(this.BuildPreferenceEntry<INumericPreferenceEntry>(preference, entry));
                    }

                    readSharedProperties(preference);
                }
                else
                {
                    var preference = new StringPreference();

                    preference.MinValue = XElementHelper.ReadStringValue(foundPreference, "minValue", false);
                    preference.MaxValue = XElementHelper.ReadStringValue(foundPreference, "maxValue", false);
                    preference.Value = XElementHelper.ReadStringValue(foundPreference, "value", false);

                    // Read the list of entryOption tags
                    var foundEntries = XElementHelper.ReadEnumerable(foundPreference, "entryOption", false);

                    // For each tag, read the values into a new PreferenceEntry object
                    foreach (var entry in foundEntries)
                    {
                        preference.Entries.Add(this.BuildPreferenceEntry<IStringPreferenceEntry>(preference, entry));
                    }

                    readSharedProperties(preference);
                }
            }

            return category as T;
        }

        /// <summary>
        /// Builds a preference entry for the provided parent based on the provided XElement node
        /// </summary>
        /// <param name="parent">The parent.</param>
        /// <param name="foundEntry">The found entry.</param>
        /// <returns></returns>
        private TPreferenceEntry BuildPreferenceEntry<TPreferenceEntry>(IPreference parent, XElement foundEntry)
            where TPreferenceEntry : class, IPreferenceEntry
        {
            // Rather ugly way of reading shared properties + returning the newly created entry
            // Several unplesant compromises here
            Func<IPreferenceEntry, TPreferenceEntry> readSharedProperties = (entry) =>
            {
                entry.FriendlyName = XElementHelper.ReadStringValue(foundEntry, "friendlyName");
                entry.Description = XElementHelper.ReadStringValue(foundEntry, "description", false);
                entry.IsSelected = XElementHelper.ReadBoolValue(foundEntry, "isDefault", false);

                //if (isSelectedByDefault && parent.HasDirectlyEditableValue)
                //{
                //    /*
                //     * This preference has both a list of pre-defined entries - one of which are selected by default - 
                //     * and a default value specified in the config file. There is really no obvious way to know which
                //     * the user actually wanted to be the default value, so log this as a warning, and let the 
                //     * pre-defined option override the directly specified default value.
                //     */

                //    parent.Value = name;
                //}

                return entry as TPreferenceEntry;
            };

            if (parent is INumericPreference)
            {
                var entry = new NumericPreferenceEntry
                {
                    Parent = (INumericPreference) parent,
                    Name = XElementHelper.ReadDoubleValue(foundEntry, "name")
                };

                return readSharedProperties(entry);
            }
            else if (parent is IDatePreference)
            {
                string dateFormat = ((IDatePreference)parent).DateFormat;
                
                var entry = new DatePreferenceEntry
                {
                    Parent = (IDatePreference) parent,
                    Name = XElementHelper.ReadDateValue(foundEntry, "name", dateFormat)
                };

                return readSharedProperties(entry);
            }
            else if (parent is IStringPreference)
            {
                var entry = new StringPreferenceEntry
                {
                    Parent = (IStringPreference) parent,
                    Name = XElementHelper.ReadStringValue(foundEntry, "name")
                };

                return readSharedProperties(entry);
            }
            else
            {
                throw new NotSupportedException("Invalid parent object");
            }
        }
    }
}

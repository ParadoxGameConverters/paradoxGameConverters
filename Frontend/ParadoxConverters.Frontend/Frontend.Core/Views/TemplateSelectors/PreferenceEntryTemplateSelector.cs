using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace Frontend.Core.Views.TemplateSelectors
{
    public class PreferenceEntryTemplateSelector : DataTemplateSelector
    {
        //  Type,   , bool(edit), bool(predef)

        //  INumeric, true      , true  -> "NumericPredefinedWithOverridePreferenceEntryTemplate"
        //  INumeric, true      , false -> "NumericEntryTemplate"
        //  INumeric, false     , true
        //  INumeric, false     , false

        //  IDate, true      , true
        //  IDate, true      , false
        //  IDate, false     , true
        //  IDate, false     , false

        //  IString, true      , true
        //  IString, true      , false  -> "PlainStringEntryTemplate"
        //  IString, false     , true   -> "PredefinedStringEntryTemplate"
        //  IString, false     , false

        /// <summary>
        /// Backing field for the known templates lookup table
        /// </summary>
        Dictionary<Tuple<Type, bool, bool>, string> knownTemplates;

        /// <summary>
        /// Backing field for the parent types lookup table
        /// </summary>
        Dictionary<Type, Type> parentTypes;

        /// <summary>
        /// This dictionary serves as the lookup table for the names for known templates
        /// </summary>
        private Dictionary<Tuple<Type, bool, bool>, string> KnownTemplates
        {
            get
            {
                return this.knownTemplates ?? (this.knownTemplates = new Dictionary<Tuple<Type, bool, bool>, string>()
                    {
                        { new Tuple<Type, bool, bool>(typeof(INumericPreferenceEntry), true, true), "NumericPredefinedWithOverridePreferenceEntryTemplate" },
                        { new Tuple<Type, bool, bool>(typeof(INumericPreferenceEntry), true, false), "NumericEntryTemplate" },
                        { new Tuple<Type, bool, bool>(typeof(IStringPreferenceEntry), true, false), "PlainStringEntryTemplate" },
                        { new Tuple<Type, bool, bool>(typeof(IStringPreferenceEntry), false, true), "PredefinedStringEntryTemplate" },
                    });
            }
        }
        
        /// <summary>
        /// This dictionary serves as a lookup table for the type of the parent objects.
        /// </summary>
        private Dictionary<Type, Type> ParentTypes
        {
            get
            {
                return this.parentTypes ?? (this.parentTypes = new Dictionary<Type, Type>()
                {
                    { typeof(INumericPreferenceEntry), typeof(INumericPreference) },
                    { typeof(IDatePreferenceEntry), typeof(IDatePreference) },
                    { typeof(IStringPreferenceEntry), typeof(IStringPreference) }
                });
            }
        }

        /// <summary>
        /// When overridden in a derived class, returns a <see cref="T:System.Windows.DataTemplate" /> based on custom logic.
        /// </summary>
        /// <param name="item">The data object for which to select the template.</param>
        /// <param name="container">The data-bound object.</param>
        /// <returns>
        /// Returns a <see cref="T:System.Windows.DataTemplate" /> or null. The default value is null.
        /// </returns>
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            FrameworkElement element = container as FrameworkElement;
            IPreferenceEntry entry = item as IPreferenceEntry;

            if (element == null || entry == null)
            {
                return null;
            }

            Type entryType = entry.GetType();
            //var parent = ((entryType)entry).Parent;

            //return this.KnownTemplates[new Tuple<Type,bool,bool>(entryType, 

            return null;
        }
    }
}

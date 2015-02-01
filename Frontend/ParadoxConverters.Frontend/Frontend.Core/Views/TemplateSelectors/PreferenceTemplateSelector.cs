using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Windows;
using System.Windows.Controls;

namespace Frontend.Core.Views.TemplateSelectors
{
    /// <summary>
    /// TemplateSelector for the IPreference objects, used to determine how to display them in the dynamically built preference views
    /// </summary>
    public class PreferenceTemplateSelector : DataTemplateSelector
    {
        /// <summary>
        /// When overridden in a derived class, returns a <see cref="T:System.Windows.DataTemplate" /> based on custom logic.
        /// </summary>
        /// <param name="item">The data object for which to select the template.</param>
        /// <param name="container">The data-bound object.</param>
        /// <returns>
        /// Returns a <see cref="T:System.Windows.DataTemplate" /> or null. The default value is null.
        /// </returns>
        /// <exception cref="System.NotImplementedException">PreferenceTemplateSelector support missing for this type of preference</exception>
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            FrameworkElement element = container as FrameworkElement;
            IPreference preference = item as IPreference;

            if (element == null || preference == null)
            {
                return null;
            }

            if (preference.HasPreDefinedChoices && preference.HasDirectlyEditableValue)
            {
                //Predefined choices, directly editable values
                return element.FindResource("PredefinedValuesWithDirectOverrideTemplate") as DataTemplate;
            }
            else if (preference.HasPreDefinedChoices && !preference.HasDirectlyEditableValue)
            {
                // Only predefined choices - regardless of whether they're numeric or not.
                return element.FindResource("PredefinedItemsPreferenceTemplate") as DataTemplate;
            }
            else if (!preference.HasPreDefinedChoices && preference.HasDirectlyEditableValue && preference is INumericPreference)
            {
                // Numeric value the user can edit.
                return element.FindResource("NumericPreferenceTemplate") as DataTemplate;
            }
            else if (!preference.HasPreDefinedChoices && preference.HasDirectlyEditableValue && !(preference is INumericPreference))
            {
                // Plain string that the user can edit
                return element.FindResource("PlainStringPreferenceTemplate") as DataTemplate;
            }
            else if (preference is IDatePreference)
            {
                // Date value the user can edit
                return element.FindResource("DatePreferenceTemplate") as DataTemplate;
            }
            else
            {
                throw new NotImplementedException("PreferenceTemplateSelector support missing for this type of preference");
            }
        }
    }
}

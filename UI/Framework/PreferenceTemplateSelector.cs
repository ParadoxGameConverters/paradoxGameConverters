using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using Converter.UI.Settings;

namespace Converter.UI.Framework
{
    public class PreferenceTemplateSelector : DataTemplateSelector
    {
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
            else if (!preference.HasPreDefinedChoices && preference.HasDirectlyEditableValue && preference.IsNumeric)
            {
                // Numeric value the user can edit.
                return element.FindResource("NumericPreferenceTemplate") as DataTemplate;
            }
            else if (!preference.HasPreDefinedChoices && preference.HasDirectlyEditableValue && !preference.IsNumeric)
            {
                // Plain string that the user can edit
                return element.FindResource("PlainStringPreferenceTemplate") as DataTemplate;
            }
            else
            {
                throw new NotImplementedException("PreferenceTemplateSelector support missing for this type of preference");
            }
        }
    }
}

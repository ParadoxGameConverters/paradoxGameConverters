using System.Windows;
using System.Windows.Controls;
using Converter.UI.Settings;

namespace Converter.UI.Framework
{
    public class EntryTemplateSelector : DataTemplateSelector
    {
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            FrameworkElement element = container as FrameworkElement;
            IPreferenceEntry entry = item as IPreferenceEntry;

            if (element == null || entry == null)
            {
                return null;
            }

            if (entry.Parent.HasPreDefinedChoices && !entry.Parent.HasDirectlyEditableValue)
            {
                // Only predefined choices - regardless of whether they're numeric or not.
                return element.FindResource("PredefinedStringEntryTemplate") as DataTemplate;
            }
            else if (!entry.Parent.HasPreDefinedChoices && entry.Parent.HasDirectlyEditableValue && entry.Parent.IsNumeric)
            {
                // Numeric value the user can edit.
                return element.FindResource("NumericEntryTemplate") as DataTemplate;
            }
            else if (entry.Parent.HasPreDefinedChoices && entry.Parent.HasDirectlyEditableValue && !entry.Parent.IsNumeric)
            {
                //Predefined choices, directly editable values and is NOT numeric.
            }
            else if (entry.Parent.HasPreDefinedChoices && entry.Parent.HasDirectlyEditableValue && entry.Parent.IsNumeric)
            {
                //Predefined choices, directly editable values and is numeric.
                return element.FindResource("NumericPredefinedWithOverridePreferenceEntryTemplate") as DataTemplate;
            }
            else if (!entry.Parent.HasPreDefinedChoices && entry.Parent.HasDirectlyEditableValue && !entry.Parent.IsNumeric)
            {
                // Plain string that the user can edit
                return element.FindResource("PlainStringEntryTemplate") as DataTemplate;
            }

            return null;
        }
    }
}

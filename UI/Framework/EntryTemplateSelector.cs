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

            if (element == null || item == null)
            {
                return null;
            }

            if (item is PredefinedPreferenceEntry)
            {
                return element.FindResource("PredefinedStringEntryTemplate") as DataTemplate;
            }

            else if (item is NumericPreferenceEntry)
            {
                return element.FindResource("NumericEntryTemplate") as DataTemplate;
            }

            else if (item is PlainStringPreferenceEntry)
            {
                return element.FindResource("PlainStringEntryTemplate") as DataTemplate;
            }

            else if (item is NumericPredefinedWithOverridePreferenceEntry)
            {
                return element.FindResource("NumericPredefinedWithOverridePreferenceEntryTemplate") as DataTemplate;
            }

            else
            {
                return null;
            }
        }
    }
}

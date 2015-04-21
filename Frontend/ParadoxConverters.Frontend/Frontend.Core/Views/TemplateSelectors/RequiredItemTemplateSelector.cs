using System.Windows;
using System.Windows.Controls;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Views.TemplateSelectors
{
    public class RequiredItemTemplateSelector : DataTemplateSelector
    {
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            var element = container as FrameworkElement;

            if (element == null)
            {
                return null;
            }

            var requiredFolder = item as IRequiredFolder;

            if (requiredFolder != null)
            {
                return element.FindResource("FolderSelectionTemplate") as DataTemplate;
            }

            var requiredFile = item as IRequiredFile;

            if (requiredFile != null)
            {
                return element.FindResource("FileSelectionTemplate") as DataTemplate;
            }

            return null;
        }
    }
}
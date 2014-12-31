using Frontend.Core.Model.Paths.Interfaces;
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
    public class RequiredItemTemplateSelector : DataTemplateSelector
    {
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            FrameworkElement element = container as FrameworkElement;
            
            if (element == null)
            {
                return null;
            }

            IRequiredFolder requiredFolder = item as IRequiredFolder;

            if (requiredFolder != null)
            {
                return element.FindResource("FolderSelectionTemplate") as DataTemplate;
            }

            IRequiredFile requiredFile = item as IRequiredFile;

            if (requiredFile != null)
            {
                return element.FindResource("FileSelectionTemplate") as DataTemplate;
            }

            return null;
        }
    }
}

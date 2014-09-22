using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Interactivity;
using System.Windows.Media;

namespace Frontend.Core.Views.Behaviours
{
    /// <summary>
    /// The ScrollIntoViewBehavior. This behavior makes the datagrid scroll all the way down every time a new row is added to its item source.
    /// </summary>
    public class ScrollIntoViewBehaviour : Behavior<DataGrid>
    {
        protected override void OnAttached()
        {
            base.OnAttached();

            // There are perhaps better events to listen to, but for the small amount of data we're dealing with here, this is sufficient
            this.AssociatedObject.LoadingRow += AssociatedObject_LoadingRow;
        }

        void AssociatedObject_LoadingRow(object sender, DataGridRowEventArgs e)
        {
            if (sender is DataGrid)
            {
                DataGrid grid = (sender as DataGrid);

                // The trick is to get to the scrollviewer inside the datagrid, and scroll that all the way down:
                var border = VisualTreeHelper.GetChild(grid, 0) as Decorator;

                if (border != null)
                {
                    var scrollViewer = border.Child as ScrollViewer;

                    if (scrollViewer != null)
                    {
                        scrollViewer.ScrollToEnd();
                    }
                }
            }
        }

        protected override void OnDetaching()
        {
            base.OnDetaching();
            this.AssociatedObject.LoadingRow -= AssociatedObject_LoadingRow;
        }
    }

}

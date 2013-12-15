using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Converter.UI.Framework;

namespace Converter.UI.Views
{
    /// <summary>
    /// Interaction logic for LogView.xaml
    /// </summary>
    public partial class LogView : ViewModelUserControl, IView
    {
        public LogView()
        {
            InitializeComponent();
        }

        public void ScrollDataGridToEnd()
        {
            var border = VisualTreeHelper.GetChild(this.LogDataGrid, 0) as Decorator;

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
}

using System.Text;
using System.Windows.Input;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Converter.UI.Commands;
using System.Linq;
using System.Windows.Controls;

namespace Converter.UI.ViewModels
{
    /// <summary>
    /// Currently unused summary viewmodel. Belongs to the equally unused SummaryView
    /// </summary>
    public class SummaryViewModel : BaseViewModel, IViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="SummaryViewModel"/> class.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="options">The options.</param>
        public SummaryViewModel(IView view, ConverterOptions options)
            : base(view, "Summary", options)
        {
        }
    }
}

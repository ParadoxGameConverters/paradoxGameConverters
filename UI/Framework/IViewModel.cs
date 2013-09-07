using System.Windows.Controls;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Interface for the viewmodels used in this project
    /// </summary>
    public interface IViewModel
    {
        /// <summary>
        /// Gets the view.
        /// </summary>
        /// <value>
        /// The view.
        /// </value>
        IView View { get; }

        /// <summary>
        /// Activates the tab.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        void ActivateTab(object sender, SelectionChangedEventArgs e);

        /// <summary>
        /// Deactivates the tab.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        void DeactivateTab(object sender, SelectionChangedEventArgs e);
    }
}

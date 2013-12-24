using System.Windows.Controls;
using Converter.UI.Settings;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Base class for all the viewmodels used in this project.
    /// </summary>
    public abstract class BaseViewModel : NotifiableBase, IViewModel
    {
        private IView view;

        /// <summary>
        /// Initializes a new instance of the <see cref="BaseViewModel"/> class.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="header">The header.</param>
        /// <param name="options">The options.</param>
        public BaseViewModel(IView view, string header, ConverterOptions options)
        {
            this.View = view;
            this.Header = header;
            this.Options = options;
        }

        /// <summary>
        /// Gets the header.
        /// </summary>
        /// <value>
        /// The header.
        /// </value>
        public string Header { get; private set; }

        /// <summary>
        /// Gets the options.
        /// </summary>
        /// <value>
        /// The options.
        /// </value>
        public ConverterOptions Options { get; private set; }

        /// <summary>
        /// Gets the view.
        /// </summary>
        /// <value>
        /// The view.
        /// </value>
        public IView View
        {
            get
            {
                return this.view;
            }

            private set
            {
                this.view = value;
                this.view.ViewModel = this;
            }
        }

        /// <summary>
        /// Activates the tab.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        public void ActivateTab(object sender, SelectionChangedEventArgs e)
        {
            this.OnTabSelected(sender, e);
        }

        /// <summary>
        /// Deactivates the tab.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        public void DeactivateTab(object sender, SelectionChangedEventArgs e)
        {
            this.OnTabSelected(sender, e);
        }

        /// <summary>
        /// Called when [tab deselected].
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        protected virtual void OnTabDeselected(object sender, SelectionChangedEventArgs e)
        {
        }

        /// <summary>
        /// Called when [tab selected].
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
        protected virtual void OnTabSelected(object sender, SelectionChangedEventArgs e)
        {
        }
    }
}

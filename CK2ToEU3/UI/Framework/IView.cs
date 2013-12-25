
namespace Converter.UI.Framework
{
    /// <summary>
    /// Interface for the views used in this project
    /// </summary>
    public interface IView
    {
        /// <summary>
        /// Gets or sets the view model.
        /// </summary>
        /// <value>
        /// The view model.
        /// </value>
        IViewModel ViewModel { get; set; }
    }
}

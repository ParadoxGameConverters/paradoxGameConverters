
namespace Frontend.Core.Model.Interfaces
{
    public interface IMod
    {
        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        string Name { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether this object representes a dummy item (that is, not a real mod)
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is dummy item]; otherwise, <c>false</c>.
        /// </value>
        bool IsDummyItem { get; set; }
    }
}

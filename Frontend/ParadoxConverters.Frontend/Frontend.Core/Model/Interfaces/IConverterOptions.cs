namespace Frontend.Core.Model.Interfaces
{
    public interface IConverterOptions
    {
        /// <summary>
        ///     Gets or sets the current converter settings
        /// </summary>
        IConverterSettings CurrentConverter { get; set; }

        /// <summary>
        ///     Gets or sets a value indicating whether conversion was successful.
        /// </summary>
        /// <value>
        ///     <c>true</c> if conversion was successful; otherwise, <c>false</c>.
        /// </value>
        bool WasConversionSuccessful { get; set; }
    }
}
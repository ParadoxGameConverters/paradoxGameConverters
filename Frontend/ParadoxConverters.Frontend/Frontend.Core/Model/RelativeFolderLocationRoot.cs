namespace Frontend.Core.Model
{
    /// <summary>
    ///     Enum used to, well, enumerate the different types of save game locations the converter UI can handle
    /// </summary>
    public enum RelativeFolderLocationRoot
    {
        /// <summary>
        ///     The windows users folder
        /// </summary>
        WindowsUsersFolder,

        /// <summary>
        ///     The steam folder
        /// </summary>
        SteamFolder,

        /// <summary>
        ///     The folder where the converter itself is located
        /// </summary>
        ConverterFolder
    }
}
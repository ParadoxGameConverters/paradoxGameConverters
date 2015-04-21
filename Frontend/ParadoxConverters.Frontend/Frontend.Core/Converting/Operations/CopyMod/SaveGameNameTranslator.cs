using System.Collections.Generic;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    /// <summary>
    ///     The converter tool normally creates an output mod named the same as the save game it was created from.
    ///     There are, however, a couple of exceptions:
    ///     The characters ' ' (whitespace) and '-' (dash) are replaced with '_' (underscore).
    ///     This class is responsible for adjusting the file/folder path adjustments in the frontend
    /// </summary>
    public class SaveGameNameTranslator : ISaveGameNameTranslator
    {
        public string TranslateName(string name)
        {
            var charsToReplace = new List<char> {' ', '-'};
            var result = name;

            charsToReplace.ForEach(c => result = result.Replace(c, '_'));

            return result;
        }
    }
}
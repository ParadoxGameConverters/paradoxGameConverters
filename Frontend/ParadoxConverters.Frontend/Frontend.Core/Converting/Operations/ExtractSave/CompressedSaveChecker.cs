using System.IO;

namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public class CompressedSaveChecker : ICompressedSaveChecker
    {
        public bool IsCompressedSave(string path)
        {
            try
            {
                using (StreamReader reader = new StreamReader(path))
                {
                    var firstLine = reader.ReadLine();

                    if (firstLine.StartsWith("PK"))
                    {
                        return true;
                    }

                    return false;
                }
            }
            catch
            {
                return false;
            }
        }
    }
}

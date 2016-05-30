using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Helpers
{
    public interface IOutputConfigurationFileHelper
    {
        string BuiltOutputString(IConverterSettings converterSettings, IDirectoryHelper directoryHelper);
        string ReadTextFile(string path);
    }
}
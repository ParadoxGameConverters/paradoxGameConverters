using System;
namespace Frontend.Core.Helpers
{
    public interface IOutputConfigurationFileHelper
    {
        string BuiltOutputString(Frontend.Core.Model.Interfaces.IConverterSettings converterSettings, IDirectoryHelper directoryHelper);
        string ReadTextFile(string path);
    }
}

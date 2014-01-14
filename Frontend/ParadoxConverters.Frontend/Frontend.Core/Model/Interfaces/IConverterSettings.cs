using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Interfaces
{
    public interface IConverterSettings : IModel
    {
        string Name { get; set; }
        string FriendlyName { get; set; }
        string DefaultConfigurationFile { get; set; }
        string UserConfigurationFile { get; set; }
        string ConverterExeName { get; set; }
        bool IsSelected { get; set; }
        IGameConfiguration SourceGame { get; set; }
        IGameConfiguration TargetGame { get; set; }
    }
}

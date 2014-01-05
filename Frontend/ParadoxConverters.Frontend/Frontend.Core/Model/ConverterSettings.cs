using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model
{
    public class ConverterSettings
    {
        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string DefaultConfigurationFile { get; set; }
        public string UserConfigurationFile { get; set; }
    }
}

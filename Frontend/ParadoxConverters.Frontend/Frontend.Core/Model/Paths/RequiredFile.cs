using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFile : RequiredItemBase, IRequiredFile
    {
        public RequiredFile(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string extension, string predefinedFileName, string internalTagName)
            : base(tagName, friendlyName, description, alternatives, internalTagName)
        {
            this.Extension = extension;
            this.PredefinedFileName = predefinedFileName;
        }

        public string Extension { get; private set; }

        public string PredefinedFileName { get; private set; }
    }
}

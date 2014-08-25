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
        public RequiredFile(string tagName, string friendlyName, string description, string defaultValue, string extension, string expectedFileName)
            : base(tagName, friendlyName, description, defaultValue)
        {
            this.Extension = extension;
            this.ExpectedFileName = expectedFileName;
        }

        public string Extension { get; private set; }

        public string ExpectedFileName { get; private set; }
    }
}

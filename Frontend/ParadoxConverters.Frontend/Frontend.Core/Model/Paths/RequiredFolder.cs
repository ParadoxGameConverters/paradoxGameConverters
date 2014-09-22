using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFolder : RequiredItemBase, IRequiredFolder
    {
        public RequiredFolder(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string internalTagName)
            : base(tagName, friendlyName, description, alternatives, internalTagName)
        {
        }
    }
}

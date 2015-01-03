using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFolder : RequiredItemBase, IRequiredFolder
    {
        public RequiredFolder(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string internalTagName, bool isMandatory)
            : base(tagName, friendlyName, description, alternatives, internalTagName, isMandatory)
        {
        }

        public override bool IsValid
        {
            get 
            { 
                if (this.IsMandatory)
                {
                    return Directory.Exists(this.SelectedValue);
                }

                return true;
            }
        }
    }
}

using Frontend.Core.Model.Paths.Interfaces;
using System.Collections.Generic;
using System.IO;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFolder : RequiredItemBase, IRequiredFolder
    {
        public RequiredFolder(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string internalTagName, bool isMandatory)
            : base(tagName, friendlyName, description, alternatives, internalTagName, isMandatory)
        {
        }

        public override string ValidateSelectedValue()
        {
            if (this.IsMandatory)
            {
                var exists = Directory.Exists(this.SelectedValue);

                if (!exists)
                {
                    return "Directory does not exist";
                }
            }

            return null;
        }
    }
}

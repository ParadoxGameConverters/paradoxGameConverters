using System.Collections.Generic;
using System.IO;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFolder : RequiredItemBase, IRequiredFolder
    {
        public RequiredFolder(string tagName, string friendlyName, string description,
            IList<IAlternativePath> alternatives, string internalTagName, bool isMandatory, bool isHidden)
            : base(tagName, friendlyName, description, alternatives, internalTagName, isMandatory, isHidden)
        {
        }

        public override string ValidateSelectedValue()
        {
            if (IsMandatory)
            {
                var exists = Directory.Exists(SelectedValue);

                if (!exists)
                {
                    return "Directory does not exist";
                }
            }

            return null;
        }
    }
}
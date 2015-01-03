using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Paths
{
    public class RequiredFile : RequiredItemBase, IRequiredFile
    {
        public RequiredFile(string tagName, string friendlyName, string description, IList<IAlternativePath> alternatives, string extension, string predefinedFileName, string internalTagName, bool isMandatory)
            : base(tagName, friendlyName, description, alternatives, internalTagName, isMandatory)
        {
            this.Extension = extension;
            this.PredefinedFileName = predefinedFileName;
        }

        public string Extension { get; private set; }

        public string PredefinedFileName { get; private set; }

        public override bool IsValid
        {
            get
            {
                if (this.IsMandatory)
                {
                    return File.Exists(this.SelectedValue);
                }

                return true;
            }
        }
    }
}

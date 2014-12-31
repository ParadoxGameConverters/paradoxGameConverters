using System;
using System.Collections.Generic;
namespace Frontend.Core.Model.Paths.Interfaces
{
    public interface IRequiredItemBase
    {
        string FriendlyName { get; }
        string SelectedValue { get; set; }
        string TagName { get; }
        string InternalTagName { get; }
        string Description { get;}
        IList<IAlternativePath> AlternativePaths { get; }
        string DefaultValue { get; }
    }
}

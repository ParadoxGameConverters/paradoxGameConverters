using System;
namespace Frontend.Core.Model.Paths.Interfaces
{
    public interface IRequiredItemBase
    {
        string FriendlyName { get; }
        string DefaultValue { get; }
        string SelectedValue { get; set; }
        string TagName { get; }
        string Description { get;}
    }
}

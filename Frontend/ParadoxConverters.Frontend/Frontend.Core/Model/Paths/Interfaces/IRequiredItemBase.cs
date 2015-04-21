using System.Collections.Generic;
using System.ComponentModel;

namespace Frontend.Core.Model.Paths.Interfaces
{
    public interface IRequiredItemBase : INotifyPropertyChanged
    {
        string FriendlyName { get; }
        string SelectedValue { get; set; }
        string TagName { get; }
        string InternalTagName { get; }
        string Description { get; }
        IList<IAlternativePath> AlternativePaths { get; }
        string DefaultValue { get; }
        bool IsMandatory { get; }
        bool IsValid { get; }
    }
}
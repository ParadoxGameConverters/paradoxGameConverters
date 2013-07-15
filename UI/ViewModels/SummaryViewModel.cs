using System.Text;
using System.Windows.Input;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Converter.UI.Commands;
using System.Linq;
using System.Windows.Controls;

namespace Converter.UI.ViewModels
{
    public class SummaryViewModel : BaseViewModel, IViewModel
    {
        //private string summary;

        public SummaryViewModel(IView view, ConverterOptions options)
            : base(view, "Summary", options)
        {
        }

        ////public string Summary
        ////{
        ////    get
        ////    {
        ////        return this.Options.OutputConfiguration;
        ////    }

        ////    //set
        ////    //{
        ////    //    if (this.summary == value)
        ////    //    {
        ////    //        return;
        ////    //    }

        ////    //    this.summary = value;
        ////    //    this.RaisePropertyChanged("Summary");
        ////    //}
        ////}

        protected override void OnTabSelected(object sender, SelectionChangedEventArgs e)
        {
        }
    }
}

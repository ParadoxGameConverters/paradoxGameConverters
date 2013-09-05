using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using Converter.UI.Factories;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Converter.UI.ViewModels;
using Converter.UI.Views;
using System.Windows.Controls;
using System.Linq;
using System.Threading;
using System;
using System.Globalization;
using System.Windows.Markup;

namespace Converter.UI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private ObservableCollection<LogEntry> logItems;

        private IList<IViewModel> tabs;

        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = this;
        }

        #region [ Properties ]

        public ObservableCollection<LogEntry> LogItems
        {
            get
            {
                return this.logItems ?? (this.logItems = new ObservableCollection<LogEntry>());
            }
        }

        public IList<IViewModel> Tabs
        {
            get
            {
                return this.tabs ?? (this.tabs = new List<IViewModel>());
            }
        }

        public ConverterOptions ConverterOptions { get; set; }

        public string ConverterTitle
        {
            get
            {
                return this.ConverterOptions.SourceGame.FriendlyName + " to " + this.ConverterOptions.TargetGame.FriendlyName + " converter";
            }
        }

        #endregion

        #region [ Methods ]

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            //Thread.CurrentThread.CurrentCulture = Thread.CurrentThread.CurrentUICulture;
            FrameworkElement.LanguageProperty.OverrideMetadata(typeof(FrameworkElement),new FrameworkPropertyMetadata(XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag)));

            this.ConverterOptions = new ConverterOptions();

            // Add tabs. The first tab will display the paths tab, and will be selected by default.
            this.Tabs.Add(new PathPickerViewModel(this.ConverterOptions, new PathPickerView()));
            this.tabControl.SelectedIndex = 0;

            // Add one or more preference views
            var factory = new PreferenceViewModelFactory(this.ConverterOptions);

            foreach (var viewModel in factory.BuildPreferenceViewModels())
            {
                this.Tabs.Add(viewModel);
            }

            //this.Tabs.Add(new SummaryViewModel(new SummaryView(), this.ConverterOptions));
            this.Tabs.Add(new LogViewModel(new LogView(), this.ConverterOptions));

            this.tabControl.SelectionChanged += new SelectionChangedEventHandler(tabControl_SelectionChanged);
        }

        private void tabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                IViewModel addedItems = e.AddedItems[0] as IViewModel;

                if (addedItems != null)
                {
                    addedItems.ActivateTab(sender, e);
                }
            }

            if (e.RemovedItems.Count > 0)
            {
                IViewModel removedItems = e.RemovedItems[0] as IViewModel;

                if (removedItems != null)
                {
                    removedItems.DeactivateTab(sender, e);
                }
            }
        }

        #endregion
    }
}

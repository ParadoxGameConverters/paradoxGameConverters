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
        /// <summary>
        /// The log items collection
        /// </summary>
        private ObservableCollection<LogEntry> logItems;

        /// <summary>
        /// The tabs
        /// </summary>
        private IList<IViewModel> tabs;

        /// <summary>
        /// Initializes a new instance of the <see cref="MainWindow"/> class.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = this;
        }

        #region [ Properties ]

        /// <summary>
        /// Gets the collection of log entries recorded so far.
        /// </summary>
        /// <value>
        /// The log items.
        /// </value>
        public ObservableCollection<LogEntry> LogItems
        {
            get
            {
                return this.logItems ?? (this.logItems = new ObservableCollection<LogEntry>());
            }
        }

        /// <summary>
        /// Gets the list of tabs to display
        /// </summary>
        /// <value>
        /// The tabs.
        /// </value>
        public IList<IViewModel> Tabs
        {
            get
            {
                return this.tabs ?? (this.tabs = new List<IViewModel>());
            }
        }

        /// <summary>
        /// Gets or sets the converter options object that contains pretty much all the user choices and configuration options the converter needs.
        /// </summary>
        /// <value>
        /// The converter options.
        /// </value>
        public ConverterOptions ConverterOptions { get; set; }

        /// <summary>
        /// Gets the title of the converter, as displayed in the title bar.
        /// </summary>
        /// <value>
        /// The converter title.
        /// </value>
        public string ConverterTitle
        {
            get
            {
                return this.ConverterOptions.SourceGame.FriendlyName + " to " + this.ConverterOptions.TargetGame.FriendlyName + " converter";
            }
        }

        #endregion

        #region [ Methods ]

        /// <summary>
        /// Raises the <see cref="E:System.Windows.FrameworkElement.Initialized" /> event. This method is invoked whenever <see cref="P:System.Windows.FrameworkElement.IsInitialized" /> is set to true internally.
        /// </summary>
        /// <param name="e">The <see cref="T:System.Windows.RoutedEventArgs" /> that contains the event data.</param>
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);
            
            // Set the frontend culture to the users current culture.
            FrameworkElement.LanguageProperty.OverrideMetadata(typeof(FrameworkElement),new FrameworkPropertyMetadata(XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag)));

            this.ConverterOptions = new ConverterOptions();

            // Add tabs. The first tab will display the paths tab, and will be selected by default.
            this.Tabs.Add(new PathPickerViewModel(this.ConverterOptions, new PathPickerView()));
            this.tabControl.SelectedIndex = 0;

            // Add one or more preference tabs
            var factory = new PreferenceViewModelFactory(this.ConverterOptions);

            foreach (var viewModel in factory.BuildPreferenceViewModels())
            {
                this.Tabs.Add(viewModel);
            }

            // Currently not used: Summary tab
            //this.Tabs.Add(new SummaryViewModel(new SummaryView(), this.ConverterOptions));

            // Add the log tab
            this.Tabs.Add(new LogViewModel(new LogView(), this.ConverterOptions));

            // Listen for tab selection changes, as some tabs needs to know
            this.tabControl.SelectionChanged += new SelectionChangedEventHandler(tabControl_SelectionChanged);
        }

        /// <summary>
        /// Handles the SelectionChanged event of the tabControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="SelectionChangedEventArgs"/> instance containing the event data.</param>
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

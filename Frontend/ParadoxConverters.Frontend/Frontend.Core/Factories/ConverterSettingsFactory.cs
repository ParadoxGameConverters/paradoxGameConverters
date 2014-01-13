using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class ConverterSettingsFactory : FactoryBase
    {
        private GameConfigurationFactory gameConfigurationFactory;

        public ConverterSettingsFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "converter")
        {
        }

        protected GameConfigurationFactory GameConfigurationFactory
        {
            get
            {
                return this.gameConfigurationFactory ?? (this.gameConfigurationFactory = new GameConfigurationFactory(this.EventAggregator));
            }
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            var defaultConfigurationFile = XElementHelper.ReadStringValue(element, "defaultConfigurationFile");
            var converterExeName = XElementHelper.ReadStringValue(element, "converterExeName");
            //var userConfigurationFile = XElementHelper.ReadStringValue(element, "userConfigurationFile");
            var isDefault = XElementHelper.ReadBoolValue(element, "isDefault");

            return new ConverterSettings(this.EventAggregator) 
            { 
                Name = name, 
                FriendlyName = friendlyName, 
                DefaultConfigurationFile = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), defaultConfigurationFile), 
                IsSelected = isDefault,
                ConverterExeName = converterExeName
                //UserConfigurationFile = userConfigurationFile 
            } as T;
        }
    }
}

using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class ConverterSettingsFactory : FactoryBase
    {
        public ConverterSettingsFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "converter")
        {
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            var defaultConfigurationFile = XElementHelper.ReadStringValue(element, "defaultConfigurationFile");
            //var userConfigurationFile = XElementHelper.ReadStringValue(element, "userConfigurationFile");
            var isDefault = XElementHelper.ReadBoolValue(element, "isDefault");

            return new ConverterSettings() 
            { 
                Name = name, 
                FriendlyName = friendlyName, 
                DefaultConfigurationFile = defaultConfigurationFile, 
                IsSelected = isDefault
                //UserConfigurationFile = userConfigurationFile 
            } as T;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace Frontend.Core.Views.ValueConverters
{
    public class BooleanToVisibilityConverter : IValueConverter
    {
        private const Visibility defaultState = Visibility.Collapsed;
        private readonly Dictionary<KeyValuePair<bool, VisibleWhen>, Visibility> lookup;

        public BooleanToVisibilityConverter()
        {
            lookup = new Dictionary<KeyValuePair<bool, VisibleWhen>, Visibility>();

            lookup.Add(new KeyValuePair<bool, VisibleWhen>(true, VisibleWhen.True), Visibility.Visible);
            lookup.Add(new KeyValuePair<bool, VisibleWhen>(true, VisibleWhen.False), Visibility.Collapsed);
            lookup.Add(new KeyValuePair<bool, VisibleWhen>(false, VisibleWhen.True), Visibility.Collapsed);
            lookup.Add(new KeyValuePair<bool, VisibleWhen>(false, VisibleWhen.False), Visibility.Visible);
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
            {
                return defaultState;
            }

            var inputValue = ParseBool(value);
            var rule = ParseEnum(parameter);

            return lookup[new KeyValuePair<bool, VisibleWhen>(inputValue, rule)];
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        private VisibleWhen ParseEnum(object value)
        {
            VisibleWhen visibleWhen;
            var success = Enum.TryParse(value.ToString(), out visibleWhen);

            if (success)
            {
                return visibleWhen;
            }

            return VisibleWhen.True;
        }

        private bool ParseBool(object value)
        {
            bool result;
            var success = bool.TryParse(value.ToString(), out result);

            if (success)
            {
                return result;
            }

            return false;
        }
    }
}
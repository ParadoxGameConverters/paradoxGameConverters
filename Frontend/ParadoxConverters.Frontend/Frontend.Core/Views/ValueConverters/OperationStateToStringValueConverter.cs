using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Data;
using Frontend.Core.Converting.Operations;

namespace Frontend.Core.Views.ValueConverters
{
    public class OperationStateToStringValueConverter : IValueConverter
    {
        private const OperationState deafultOperationState = OperationState.Invalid;
        private readonly Dictionary<OperationState, string> mapper;

        public OperationStateToStringValueConverter()
        {
            mapper = new Dictionary<OperationState, string>();

            mapper.Add(OperationState.Cancelled, "Canceled");
            mapper.Add(OperationState.Failed, "Failed");
            mapper.Add(OperationState.CompleteWithWarnings, "Completed, but with warnings");
            mapper.Add(OperationState.InProgress, "In progress");
            mapper.Add(OperationState.NotStarted, "Not started");
            mapper.Add(OperationState.Success, "Completed");
            mapper.Add(OperationState.Invalid, string.Empty);
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var enumValue = ParseEnum(value);

            return mapper[enumValue];
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        private OperationState ParseEnum(object value)
        {
            if (value == null)
            {
                return deafultOperationState;
            }

            OperationState state;
            var success = Enum.TryParse(value.ToString(), out state);

            if (success)
            {
                return state;
            }

            return deafultOperationState;
        }
    }
}
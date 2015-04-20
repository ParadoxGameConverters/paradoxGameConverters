using Frontend.Core.Converting.Operations;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace Frontend.Core.Views.ValueConverters
{
    public class OperationStateToStringValueConverter : IValueConverter
    {
        private Dictionary<OperationState, string> mapper;
        private const OperationState deafultOperationState = OperationState.Invalid;

        public OperationStateToStringValueConverter()
        {
            this.mapper = new Dictionary<OperationState, string>();

            this.mapper.Add(OperationState.Cancelled, "Canceled");
            this.mapper.Add(OperationState.CompleteWithErrors, "Completed, but with errors");
            this.mapper.Add(OperationState.CompleteWithWarnings, "Completed, but with warnings");
            this.mapper.Add(OperationState.InProgress, "In progress");
            this.mapper.Add(OperationState.NotStarted, "Not started");
            this.mapper.Add(OperationState.Success, "Completed");
            this.mapper.Add(OperationState.Invalid, string.Empty);
        }

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var enumValue = this.ParseEnum(value);

            return this.mapper[enumValue];
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
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
            bool success = Enum.TryParse<OperationState>(value.ToString(), out state);

            if (success)
            {
                return state;
            }

            return deafultOperationState;
        }
    }
}

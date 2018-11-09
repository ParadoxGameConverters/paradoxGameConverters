using Frontend.Core.Model.Preferences.Interfaces;
using System.Collections.Generic;
using System.Text;

namespace Frontend.Core.Model.Preferences
{
    public class StringListPreference : Preference<List<string>>, IStringListPreference
    {
        /// <summary>
        ///     Override tostring to produce one line per entry
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            if (Value != null)
            {
                var sb = new StringBuilder();
                
                sb.AppendLine("# " + FriendlyName + ": " + Description);
                AppendEntries(sb);

                foreach (var selection in Value)
                {
                    sb.AppendLine("\t" + Name + " = \"" + selection.ToString() + "\"");
                }

                return sb.ToString();
            }

            return base.ToString();
        }
    }
}
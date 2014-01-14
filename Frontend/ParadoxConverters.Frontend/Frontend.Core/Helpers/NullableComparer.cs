using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Helpers
{
    /// <summary>
    /// I could swear c# contains something that does exactly this already, but I couldn't for the life of me:
    /// * Locate it
    /// * Remember what it's called
    /// * Find it using ddg
    /// Or anything else useful 
    /// 
    /// The only things I found wasn't able to easily support DateTime, string and double using the same mechanism.
    /// 
    /// I'm sure this approach has weaknesses I haven't thought of, but this is only meant as a temporary solution.
    /// </summary>
    public static class NullableComparer
    {
        public static bool AreEqual(object something, object somethingElse)
        {
            // This could be done better
            if (something == null && somethingElse == null)
            {
                return true;
            }
            else if (somethingElse != null && !somethingElse.Equals(something))
            {
                return false;
            }
            else if (something != null && !something.Equals(somethingElse))
            {
                return false;
            }
            else if (somethingElse.Equals(something))
            {
                return true;
            }

            return false;
        }
    }
}

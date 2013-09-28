using System.Collections.Generic;

namespace System.Linq
{
    /// <summary>
    /// Extension methods for IEnumerable
    /// </summary>
    public static class EnumerableExtensionMethods
    {
        /// <summary>
        /// Fors the each.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="source">The source.</param>
        /// <param name="action">The action.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentNullException">action</exception>
        public static IEnumerable<T> ForEach<T>(this IEnumerable<T> source, Action<T> action)
        {
            if (action == null)
            {
                throw new ArgumentNullException("action");
            }

            foreach (var item in source)
            {
                action(item);
            }

            return source;
        }

        //public static int ForEach<T>(this IEnumerable<T> list, Action<int, T> action)
        //{
        //    if (action == null)
        //    {
        //        throw new ArgumentNullException("action");
        //    }

        //    var index = 0;

        //    foreach (var elem in list)
        //    {
        //        action(index++, elem);
        //    }

        //    return index;
        //}
    }
}

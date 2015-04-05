using System;
using System.Collections.Generic;
namespace Frontend.Core.Converting.Tasks
{
    public interface ITaskProvider
    {
        IEnumerable<ITask> Tasks { get; }

        void AddTask(ITask task);
    }
}

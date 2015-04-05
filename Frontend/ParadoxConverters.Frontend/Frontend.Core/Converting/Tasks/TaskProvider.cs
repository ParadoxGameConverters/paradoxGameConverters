using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Tasks
{
    public class TaskProvider : ITaskProvider
    {
        private IList<ITask> tasks;

        public TaskProvider()
        {
            this.tasks = new List<ITask>();
        }

        public IEnumerable<ITask> Tasks
        {
            get
            {
                return this.tasks;
            }
        }

        public void AddTask(ITask task)
        {
            this.tasks.Add(task);
        }
    }
}

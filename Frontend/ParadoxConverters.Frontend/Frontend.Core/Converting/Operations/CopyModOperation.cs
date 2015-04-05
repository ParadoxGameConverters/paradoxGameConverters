using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public class CopyModOperation : IOperation
    {
        public string Description
        {
            get
            {
                return "Copying mod";
            }
        }

        public Task<OperationResult> Process()
        {
            Thread.Sleep(2000);
            var result = new OperationResult();
            return Task.FromResult<OperationResult>(result);
        }

        public bool CanRun()
        {
            return true;
        }
    }
}

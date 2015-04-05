using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public class ExtractSaveOperation : IOperation
    {
        public string Description
        {
            get
            {
                return "Extracting save";
            }
        }

        public async Task<OperationResult> Process()
        {
            Thread.Sleep(2000);
            return new OperationResult();
        }

        bool IOperation.CanRun()
        {
            return true;
        }
    }
}

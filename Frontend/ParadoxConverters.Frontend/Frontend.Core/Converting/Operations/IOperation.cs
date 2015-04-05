using Frontend.Core.Converting.Tasks;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public interface IOperation
    {
        string Description { get; }

        Task<OperationResult> Process();

        bool CanRun();
    }
}

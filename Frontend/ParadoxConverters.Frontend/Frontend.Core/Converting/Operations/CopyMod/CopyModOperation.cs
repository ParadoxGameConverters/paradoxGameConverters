using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    public class CopyModOperation : IOperation
    {
        private IModCopier modCopier;

        public CopyModOperation(IEventAggregator eventAggregator, IConverterOptions options)
        {
            this.modCopier = new ModCopier(
                eventAggregator,
                options,
                new FileProxy(),
                new FolderProxy(),
                new DirectoryHelper(),
                new MessageBoxProxy(),
                new DirectoryCopyHelper(),
                new SaveGameNameTranslator());
        }

        public string Description
        {
            get
            {
                return "Copying mod...";
            }
        }

        public Task<OperationResult> Process()
        {
            var result = this.modCopier.MoveModFileAndFolder();
            return Task.FromResult<OperationResult>(result);
        }

        public bool CanRun()
        {
            return true;
        }
    }
}

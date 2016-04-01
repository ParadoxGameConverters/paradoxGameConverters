using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    public class CopyModOperation : IOperation
    {
        private readonly IModCopier modCopier;

        public CopyModOperation(IConverterOptions options)
        {
            modCopier = new ModCopier(
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
            get { return "Copying mod..."; }
        }

        public OperationResult Process()
        {
            var result = modCopier.MoveModFileAndFolder();
            return result;
        }

        public bool CanRun()
        {
            return true;
        }
    }
}
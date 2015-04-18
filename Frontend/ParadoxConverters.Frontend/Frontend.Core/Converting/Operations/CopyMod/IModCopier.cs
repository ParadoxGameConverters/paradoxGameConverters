using Frontend.Core.Converting.Operations;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    public interface IModCopier
    {
        OperationResult MoveModFileAndFolder();
    }
}

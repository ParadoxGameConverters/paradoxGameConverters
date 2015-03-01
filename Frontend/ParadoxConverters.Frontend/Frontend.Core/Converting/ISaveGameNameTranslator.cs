using System;
namespace Frontend.Core.Converting
{
    public interface ISaveGameNameTranslator
    {
        string TranslateName(string name);
    }
}

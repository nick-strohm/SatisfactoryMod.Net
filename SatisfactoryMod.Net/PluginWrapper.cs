using System;
using System.Runtime.CompilerServices;

[assembly:RuntimeCompatibility(WrapNonExceptionThrows = true)]

namespace SatisfactoryMod.Net
{
    internal static class PluginWrapper
    {
        public static void Main()
        {
        }

        public static void PluginMain()
        {
            Console.WriteLine($"{nameof(PluginWrapper)}.{nameof(PluginMain)} invoked.");
        }
    }
}

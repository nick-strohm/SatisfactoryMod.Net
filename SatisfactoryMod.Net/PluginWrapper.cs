using System;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using SatisfactoryMod.Net.Logging.Common;

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
            var assembly = typeof(PluginWrapper).Assembly.GetName();

            Console.WriteLine($"Invoked {nameof(PluginWrapper)}.{nameof(PluginMain)} in {assembly.Name} (Version {assembly.Version})");

            var plugin = new Plugin();
            plugin.Logger.Log(LogLevel.Info, $"Starting {assembly.Name} version {assembly.Version}.");

            plugin.Prepare();

            Task.Run(async () =>
            {
                await plugin.Start();

                plugin.Logger.Log(LogLevel.Info, $"{assembly.Name} startup finished.");
            });

            Console.WriteLine($"{nameof(PluginWrapper)}.{nameof(PluginMain)} invoked.");
        }
    }
}

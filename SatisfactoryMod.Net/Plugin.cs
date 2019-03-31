using System;
using System.Threading.Tasks;
using SatisfactoryMod.Net.Logging.Common;
using SatisfactoryMod.Net.Logging.LogProvider;

namespace SatisfactoryMod.Net
{
    public class Plugin
    {
        public ILogger Logger { get; }

        public Plugin()
        {
            AppDomain.CurrentDomain.UnhandledException += OnUnhandledException;

            Logger = new NLogLogger();
        }

        private void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Logger.Log(LogLevel.Fatal, $"< ==== UNHANDLED EXCEPTION ==== > {Environment.NewLine} Received an unhandled exception from {sender?.GetType()}: ", (Exception)e.ExceptionObject);
        }

        public void Prepare()
        {
        }

        public Task Start()
        {
            return Task.CompletedTask;
        }
    }
}
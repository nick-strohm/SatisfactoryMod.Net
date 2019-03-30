using System;

namespace SatisfactoryMod.Net.Logging.Common
{
    public abstract class BaseLogger : ILogger
    {
        public int FramesToSkip { get; set; }

        public abstract void Log(LogLevel level, string message, Exception ex, params object[] args);
        public abstract void Log(LogLevel level, string message, params object[] args);
        public abstract void Log(LogLevel level, Exception ex);
    }
}
using System;

namespace SatisfactoryMod.Net.Logging.Common
{
    public interface ILogger
    {
        /// <summary>
        /// Defines how many methods the class detector should skip on a log message.
        ///
        /// This property is very useful, if you create your own wrapper that utilizes this method.
        /// </summary>
        /// 
        int FramesToSkip { get; set; }

        void Log(LogLevel level, string message, Exception ex, params object[] args);

        void Log(LogLevel level, string message, params object[] args);

        void Log(LogLevel level, Exception ex);
    }
}
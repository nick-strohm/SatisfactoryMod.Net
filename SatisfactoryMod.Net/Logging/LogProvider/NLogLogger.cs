using System;
using System.IO;
using NLog;
using NLog.Config;
using SatisfactoryMod.Net.Logging.Common;
using LogLevel = SatisfactoryMod.Net.Logging.Common.LogLevel;

namespace SatisfactoryMod.Net.Logging.LogProvider
{
    public static class NLogLoggerExtensions
    {
        public static NLog.LogLevel ToNLogLevel(this LogLevel level)
        {
            switch (level)
            {
                case LogLevel.Trace:
                    return NLog.LogLevel.Trace;
                case LogLevel.Debug:
                    return NLog.LogLevel.Debug;
                case LogLevel.Info:
                    return NLog.LogLevel.Info;
                case LogLevel.Warning:
                    return NLog.LogLevel.Warn;
                case LogLevel.Error:
                    return NLog.LogLevel.Error;
                case LogLevel.Fatal:
                    return NLog.LogLevel.Fatal;
                default:
                    throw new ArgumentOutOfRangeException(nameof(level));
            }
        }
    }

    public class NLogLogger : BaseLogger
    {
        public NLogLogger()
        {
            LogManager.Configuration = new XmlLoggingConfiguration(Path.Combine(Environment.CurrentDirectory, "dotnet/plugins/NLog.config"));
        }

        public override void Log(LogLevel level, string message, Exception ex, params object[] args)
        {
            var logger = LogManager.GetCurrentClassLogger();
            logger.Log(level.ToNLogLevel(), ex, message, args);
        }
          
        public override void Log(LogLevel level, string message, params object[] args)
        {
            var logger = LogManager.GetCurrentClassLogger();
            logger.Log(level.ToNLogLevel(), message, args);
        }

        public override void Log(LogLevel level, Exception ex)
        {
            var logger = LogManager.GetCurrentClassLogger();
            logger.Log(level.ToNLogLevel(), ex);
        }
    }
}
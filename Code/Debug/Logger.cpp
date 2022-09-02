#include "Logger.h"

Logger* Logger::GetMain()
{
	return Main;
}

Logger::Logger(std::string loggerName)
{
	std::string loggerPath = "Logs/" + loggerName + ".log";

	logger = spdlog::basic_logger_mt(loggerName, loggerPath, true);
}

void Logger::Delete() 
{
	spdlog::drop(logger.get()->name());
}

void Logger::Log(std::string message)
{
	logger->info(message);
}

void Logger::LogWarning(std::string warning)
{
	logger->warn(warning);
}

void Logger::LogError(std::string error)
{
	logger->error(error);
}

Logger* Logger::Main;

void Logger::SetMain()
{
	Main = this;
}

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

class Logger 
{
public:
	static Logger* GetMain();
	Logger(std::string loggerName);
	void Delete();
	void Log(std::string message);
	void LogWarning(std::string warning);
	void LogError(std::string error);
	void SetMain();
private:
	static Logger* Main;
	std::shared_ptr<spdlog::logger> logger;
};
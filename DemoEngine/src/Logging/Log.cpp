#include "DemoEngine_PCH.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace DemoEngine
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;
	void Log::Init()
	{
		spdlog::set_pattern("%^[%] %: %v%$");
		s_Logger = spdlog::stdout_color_mt("DemoEngine");
		s_Logger->set_level(spdlog::level::trace);
	}
}
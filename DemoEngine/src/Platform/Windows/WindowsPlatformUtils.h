#pragma once
#include "Utils/PlatformUtils.h"

namespace DemoEngine
{
	//inherits and write for each platform 
	class WindowsFileDialogs : FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

	};
}
	
#include "DemoEngine_PCH.h"
#include "PlatformUtils.h"
#include "Platform/Windows/WindowsPlatformUtils.h"

namespace DemoEngine
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		// Delegates to Windows implementation, but could use other platform implementations 
		// using approaches like 'switch' statements.
		return WindowsFileDialogs::OpenFile(filter);
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		// Delegates to Windows implementation, but could use other platform implementations 
		// // using approaches like 'switch' statements.
		return WindowsFileDialogs::SaveFile(filter);
	}
}
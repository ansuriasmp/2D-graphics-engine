#pragma once 
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace DemoEngine
{
	//inherits and write for each platform 
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter); 
		static std::string SaveFile(const char* filter);
	};

}
#pragma once
#include <Core/Core.h>

//This will create the demo engine application for us 

extern DemoEngine::Application* DemoEngine::CreateApplication();

int main(int arc, char** argv) {

	DemoEngine::Log::Init();

	printf("Demo Engine\n");

	auto app = DemoEngine::CreateApplication();

	app->Run();

	delete app;
}

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1; 
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

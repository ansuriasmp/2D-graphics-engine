#include "DemoEngine_PCH.h" 
#include "DemoEngine.h"

#include "Core/EntryPoint.h"
#include "Editor/EditorLayer.h"

namespace DemoEngine
{
	class DemoEngineApp : public Application{

	public:
		DemoEngineApp() : Application("Demo Engine Editor") {
			PushLayer(new EditorLayer());
		}

		~DemoEngineApp() {

		}
	};

	DemoEngine::Application* DemoEngine::CreateApplication() {
		return new DemoEngineApp();
	}
	
}
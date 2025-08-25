#pragma once 
#include "Scene.h"

namespace DemoEngine
{
	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void Serialise(const std::string& filePath); 
		bool Deserialise(const std::string& filePath);

	private:
		Ref<Scene> m_Scene;
	};
}
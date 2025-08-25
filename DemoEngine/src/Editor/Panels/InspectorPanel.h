#pragma once
#include "Core/Core.h" 
#include "Scene/Entity.h"

namespace DemoEngine
{
	class InspectorPanel
	{
	public:
		InspectorPanel() {};
		~InspectorPanel() = default;

		void DrawComponents(Entity entity); 
		void DrawAddComponent(Entity entity);
	
	private:
		template<typename T>
		void DrawAddComponentMenuItem(Entity entity, const char* label);
	};
}

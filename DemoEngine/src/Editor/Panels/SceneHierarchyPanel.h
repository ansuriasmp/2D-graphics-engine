#pragma once
#include "Core/Core.h"
#include "Logging/Log.h" 
#include "Scene/Scene.h" 
#include "Scene/Entity.h"
#include "InspectorPanel.h"

namespace DemoEngine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; } 
		void SetSelectedEntity(Entity entity);
	
	private:
		void DrawEntityNode(Entity entity);
		void RightClickMenu();

	private:
		Ref<Scene> m_Context; 
		Entity m_SelectionContext;
		Ref<InspectorPanel> m_InspectorPanel;
	};
	
}

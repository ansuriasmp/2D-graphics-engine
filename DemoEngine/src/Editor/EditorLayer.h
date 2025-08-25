#pragma once
#include "DemoEngine.h"
#include "Renderer/Camera/EditorCamera.h"
#include "Editor/Panels/SceneHierarchyPanel.h"
#include <filesystem>

namespace DemoEngine
{
	class EditorLayer : public Layer
	{
	public:

		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		bool m_DisplayColliders = true;

	//private:
		//bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool OnMouseButtonPressed(MouseButtonPressedEvent e);
		bool OnKeyPressed(KeyPressedEvent e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SerialiseScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();
		void OnDeleteEntity();

		void OnOverlayRender();

	private:
		EditorCamera m_EditorCamera;
		Ref<Framebuffer> m_Framebuffer;
		//Ref<Renderer3D> m_Renderer3D;
		Ref<ShaderLibrary> m_ShaderLibrary;
		//Ref<Renderer2D> m_Renderer2D;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene, m_RuntimeScene;
		std::filesystem::path m_EditorSceneFilePath;

		int m_GizmoType = -1;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		Entity m_HoveredEntity;

		SceneHierarchyPanel m_SceneHierarchyPanel;


		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Pause = 2,
			Simulate = 3
		};

		SceneState m_SceneState = SceneState::Edit;
		
	};
}


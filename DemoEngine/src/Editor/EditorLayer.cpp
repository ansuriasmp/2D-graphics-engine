#include "DemoEngine_PCH.h" 
#include "EditorLayer.h"
#include "ImGui/ImGuiLibrary.h"
#include "ImGuizmo.h"
#include "Math/Math.h"

#include "Utils/PlatformUtils.h"
#include "Scene/SceneSerialiser.h"

namespace DemoEngine
{
	// Constructor for the editor layer
	EditorLayer::EditorLayer() :
		Layer("EditorLayer")
	{

	}

	// Called when the editor layer is attached (initialization)
	void EditorLayer::OnAttach()
	{
		m_EditorCamera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

		// Setup framebuffer for scene renderin
		FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(framebufferSpec);

		// Initialize scenes
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		// Load shaders
		m_ShaderLibrary = CreateRef<ShaderLibrary>();
		m_ShaderLibrary->Load("FlatColour", "assets/shaders/FlatColourShader.glsl");


	}

	void EditorLayer::OnDetach()
	{

	}

	// Frame update logic
	void EditorLayer::OnUpdate(Timestep ts)
	{

		//Resize Framebuffer if necessary
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && //zero sized framebuffer is invalid 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		Renderer2D::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Renderer2D::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		// Scene state management
		switch (m_SceneState)
		{
		case SceneState::Edit:
			m_EditorCamera.OnUpdate(ts);
			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}

		// Entity picking logic
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportWidth = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = m_ViewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());

		}

		m_Framebuffer->Unbind();

		auto stats = Renderer2D::GetStats();
		//LOG_INFO("Draw Calls: {0}", stats.DrawCalls);



	}

	// Main ImGui UI rendering
	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		ImGuiLibrary::CreateDockspace(dockspaceOpen, "Dockspace Demo");

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}
			
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{				
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit", "Ctrl+Q"))
				{
					dockspaceOpen = false; 
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();


		//Creating new viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");


		//Gizmo toolbar
		// Toolbar height can be the height of buttons, or you can explicitly specify it 		
		float toolbarHeight = 30.0f;

		// Set the window size for the toolbar (full width and specified height)
		ImGui::BeginChild("Toolbar", ImVec2(ImGui::GetContentRegionAvail().x, toolbarHeight), true);
		ImVec2 buttonSize = { 50, toolbarHeight };

		// Button for Move Gizmo
		if (ImGui::Button("Move", buttonSize)) {
			// Activate Move Gizmo
			if (!ImGuizmo::IsUsing()) {
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}

		}

		ImGui::SameLine();

		// Button for Rotate Gizmo
		if (ImGui::Button("Rotate", buttonSize)) {
			// Activate Rotate Gizmo
			if (!ImGuizmo::IsUsing()) {
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
		}

		ImGui::SameLine();

		// Button for Scale Gizmo
		if (ImGui::Button("Scale", buttonSize)) {
			// Activate Scale Gizmo
			if (!ImGuizmo::IsUsing()) {
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
		}


		ImGui::NextColumn();
		ImGui::SameLine();
		char* buttonName = (m_SceneState == SceneState::Edit) ? "Play" : "Stop";

		if (ImGui::Button(buttonName, buttonSize))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}

		// ... add more buttons as needed
		ImGui::EndChild(); // End of toolbar child window

		ImGui::Begin("Debug Options"); // Puedes poner otro nombre si quieres

		bool showColliders = m_ActiveScene->GetShowColliders();
		if (ImGui::Checkbox("Show Colliders", &showColliders))
		{
			m_ActiveScene->SetShowColliders(showColliders);
		}

		bool shouldConnect = m_ActiveScene->m_ShouldConnectToServer;
		if (ImGui::Checkbox("Connect to ENet Server", &shouldConnect))
		{
			if (m_ActiveScene)
			{
				m_ActiveScene->m_ShouldConnectToServer = shouldConnect;

				if (shouldConnect)
					m_ActiveScene->ConectarENet();
			}
		}


		ImGui::End();


		//Mouse position for mouse picking
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		auto viewportOffset = ImGui::GetCursorScreenPos();
		m_ViewportBounds[0] = { viewportOffset.x, viewportOffset.y };
		m_ViewportBounds[1] = { viewportOffset.x + viewportPanelSize.x, viewportOffset.y + viewportPanelSize.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColourAttachmentRendererID();
		ImGui::Image((ImTextureID)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		if (m_SceneState == SceneState::Edit)
			OnOverlayRender();

		//Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(viewportOffset.x, viewportOffset.y, viewportPanelSize.x, viewportPanelSize.y);

			//Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			//Entity transform
			auto& entityTransform = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = entityTransform.GetTransform();

			//Snapping
			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapValue = 0.5f; //snap to 0.5m for translation & scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = 45.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				entityTransform.Translation = translation;
				entityTransform.Scale = scale;

				glm::vec3 deltaRotation = rotation - entityTransform.Rotation;
				entityTransform.Rotation += deltaRotation;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		//End dockspace
		ImGui::End();

	}

	// Renders overlay elements in editor mode only
	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		if (!m_ActiveScene || !m_ActiveScene->GetShowColliders())
			return;
	}


	// Processes events sent to this layer
	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	// Handles mouse button press events
	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}

		return false;
	}

	// Handles keyboard shortcuts
	bool EditorLayer::OnKeyPressed(KeyPressedEvent e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();
			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();
			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();
			if (control && !shift) 
				SaveScene();
			break;
		}

		case Key::D:
		{
			if (!control && shift)
				OnDuplicateEntity();
			break;
		}

		case Key::Delete:
		{
			OnDeleteEntity();
			break;
		}

		//Gizmo shortcuts
		case Key::Q:
			if (m_ViewportHovered)
			{
				m_GizmoType = -1; // select
			}
			break;
		case Key::W:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered) {
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			break;
		case Key::E:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered) {
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
			break;
		case Key::R:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered) {
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			break;
		}
	}

	// Create a new empty scene
	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorSceneFilePath.clear();
	}

	// Open a scene via file dialog
	void EditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("DemoEngine Scene (*.demoengine)\0*.demoengine\0");
		if (!path.empty())
		{
			OpenScene(path);
		}
	}

	// Open scene from given file path
	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		if (path.extension().string() != ".demoengine")
		{
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		m_EditorSceneFilePath = path;

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerialiser serialiser(newScene);
		if (serialiser.Deserialise(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
		}

		m_ActiveScene = m_EditorScene;
	}

	// Save the current scene
	void EditorLayer::SaveScene()
	{
		if (!m_EditorSceneFilePath.empty())
		{
			SerialiseScene(m_EditorScene, m_EditorSceneFilePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	// Prompt for a save location and save the scene
	void EditorLayer::SaveSceneAs()
	{
		//std::string path = PlatformUtils::ShowFileOpen();
		std::string path = FileDialogs::SaveFile("DemoEngine Scene (*.demoengine)\0*.demoengine\0");
		if (!path.empty())
		{
			SerialiseScene(m_EditorScene, path);
		}
		m_EditorSceneFilePath = path;
	}

	// Handles actual serialization logic
	void EditorLayer::SerialiseScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerialiser serialiser(scene);
		serialiser.Serialise(path.string());
	}
	

	// Starts runtime playback
	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_RuntimeScene = CreateRef<Scene>();
		//Copy editor scene to runtime scene
		Scene::CopyTo(m_EditorScene, m_RuntimeScene);

		m_RuntimeScene->OnRuntimeStart();

		m_ActiveScene = m_RuntimeScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	// Stops runtime playback
	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_RuntimeScene->OnRuntimeStop();
		//Unload runtime scene, i.e. delete it 
		m_RuntimeScene = nullptr;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScene->m_ShouldConnectToServer = false;
	}

	// Duplicates currently selected entity
	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Entity Duplication permitted in Edit Mode ONLY");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
		}
	}
	
	// Deletes currently selected entity
	void EditorLayer::OnDeleteEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Entity Deletion permitted in Edit Mode ONLY");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DestroyEntity(m_SceneHierarchyPanel.GetSelectedEntity());
			m_SceneHierarchyPanel.SetSelectedEntity({}); //Must clear to avoid a crash
		}
	}
}
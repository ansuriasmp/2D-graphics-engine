#include "DemoEngine_PCH.h" 
#include "InspectorPanel.h"
#include "ImGui/ImGuiLibrary.h"
#include <imgui/imgui.h>
#include "Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include "Utils/PlatformUtils.h"

namespace DemoEngine
{
	// Draws the "Add Component" button and popup menu for adding components to an entity
	void InspectorPanel::DrawAddComponent(Entity entity)
	{
		ImGui::Separator(); // Visual separator

		// Draw centered "Add Component" button
		ImGuiLibrary::DrawCentredButton("Add Component", 0.9f, []() {
			ImGui::OpenPopup("AddComponent");
			});

		// If the popup is open, show the available components
		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponentMenuItem<SpriteRendererComponent>(entity, "Sprite Renderer");
			DrawAddComponentMenuItem<CircleRendererComponent>(entity, "Circle Renderer");
			DrawAddComponentMenuItem<CameraComponent>(entity, "Camera");

			DrawAddComponentMenuItem<Rigidbody2DComponent>(entity, "Rigidbody 2D");
			DrawAddComponentMenuItem<BoxCollider2DComponent>(entity, "Box Collider 2D");
			DrawAddComponentMenuItem<CircleCollider2DComponent>(entity, "Circle Collider 2D");

			DrawAddComponentMenuItem<AudioComponent>(entity, "Audio Component");

			DrawAddComponentMenuItem<PlayerControllerComponent>(entity, "Player Controller"); // Custom player controller

			ImGui::EndPopup();
		}
	}

	// Template function for drawing menu item for a component (only if it's not already attached)
	template<typename T>
	void InspectorPanel::DrawAddComponentMenuItem(Entity entity, const char* label)
	{
		if (!entity.HasComponent<T>())
		{
			if (ImGui::MenuItem(label)) {
				entity.AddComponent<T>(); // Add the selected component
				ImGui::CloseCurrentPopup(); // Close the popup after selection
			}
		}
	}

	// Draws the UI to inspect and edit all components of an entity
	void InspectorPanel::DrawComponents(Entity entity)
	{
		// Edit tag (name) of the entity
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		// Transform Component UI
		ImGuiLibrary::DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				ImGuiLibrary::DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				ImGuiLibrary::DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				ImGuiLibrary::DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		// Camera Component UI
		ImGuiLibrary::DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.camera;

				const char* projectionStringTypes[] = { "Orthographic", "Perspective" };
				const char* currentProjectionString = projectionStringTypes[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Camera Type", currentProjectionString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionString == projectionStringTypes[i];
						if (ImGui::Selectable(projectionStringTypes[i], isSelected))
						{
							currentProjectionString = projectionStringTypes[i];
							camera.SetProjectionType((ProjectionType)i);
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Orthographic settings
				if (camera.GetProjectionType() == ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize, 0.1f)) {
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNear = camera.GetOrthographicNear();
					if (ImGui::DragFloat("Near", &orthoNear, 0.1f))
						camera.SetOrthographicNear(orthoNear);

					float orthoFar = camera.GetOrthographicFar();
					if (ImGui::DragFloat("Far", &orthoFar, 0.1f))
						camera.SetOrthographicFar(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

				// Perspective settings
				if (camera.GetProjectionType() == ProjectionType::Perspective)
				{
					float perspectiveFov = glm::degrees(camera.GetPerspectiveFOV());
					if (ImGui::DragFloat("FOV", &perspectiveFov, 0.1f))
						camera.SetPerspectiveFOV(glm::radians(perspectiveFov));

					float perspectiveNear = camera.GetPerspectiveNear();
					if (ImGui::DragFloat("Near", &perspectiveNear, 0.1f))
						camera.SetPerspectiveNear(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFar();
					if (ImGui::DragFloat("Far", &perspectiveFar, 0.1f))
						camera.SetPerspectiveFar(perspectiveFar);
				}
			});

		// Sprite Renderer UI
		ImGuiLibrary::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));
			});

		// Circle Renderer UI
		ImGuiLibrary::DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Colour", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		// Rigidbody 2D UI
		ImGuiLibrary::DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [&](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };
				int currentBodyType = static_cast<int>(component.BodyType);
				if (ImGui::Combo("Body Type", &currentBodyType, bodyTypeStrings, IM_ARRAYSIZE(bodyTypeStrings)))
				{
					component.BodyType = static_cast<b2BodyType>(currentBodyType);
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
				ImGui::DragFloat("Mass", &component.Mass.mass, 0.01f, 0.0f, 100.0f, "%.3f");
				ImGui::DragFloat2("Mass Center", reinterpret_cast<float*>(&component.Mass.center), 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Rotational Inertia", &component.Mass.rotationalInertia, 0.1f, 0.0f, 500.0f, "%.3f");

				ImGui::Checkbox("Affected by Gravity", &component.AffectedbyGravity); // Gravity affected

				if (ImGui::Button("Reset##Rigidbody"))
					component = Rigidbody2DComponent();

				ImGui::SameLine();
				if (ImGui::Button("Remove##Rigidbody"))
					entity.RemoveComponent<Rigidbody2DComponent>();
			});

		// Box Collider 2D UI
		ImGuiLibrary::DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGuiLibrary::DrawVec2Control("Offset", component.Offset);
				ImGuiLibrary::DrawVec2Control("Half Size", component.HalfSize);

				ImGui::DragFloat("Density", &component.Density, 0.05f, 0.0f, 10.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.05f, 0.0f, 1.0f);

				if (ImGui::Button("Reset to Defaults"))
					component = BoxCollider2DComponent();
			});

		// Circle Collider 2D UI
		ImGuiLibrary::DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGuiLibrary::DrawVec2Control("Offset", component.Offset);
				ImGui::DragFloat("Radius", &component.Radius, 0.05f, 0.0f, 10.0f);

				ImGui::DragFloat("Density", &component.Density, 0.05f, 0.0f, 10.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.05f, 0.0f, 1.0f);

				if (ImGui::Button("Reset to Defaults"))
					component = CircleCollider2DComponent();
			});

		// Audio Component UI
		ImGuiLibrary::DrawComponent<AudioComponent>("Audio Component", entity, [](AudioComponent& component)
			{
				ImGui::Text("Audio File");
				ImGui::SameLine();

				float fullWidth = ImGui::GetContentRegionAvail().x;
				float buttonWidth = 30.0f;
				float inputWidth = fullWidth - buttonWidth - 10.0f;

				ImGui::PushItemWidth(inputWidth);

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), component.FilePath.c_str());

				if (ImGui::InputText("##AudioFile", buffer, sizeof(buffer)))
					component.FilePath = buffer;

				ImGui::PopItemWidth();
				ImGui::SameLine();

				if (ImGui::Button("File", ImVec2(buttonWidth, 0)))
				{
					std::string path = FileDialogs::OpenFile("Audio Files (*.wav *.mp3)\0*.wav;*.mp3\0");
					if (!path.empty())
						component.FilePath = path;
				}

				ImGui::Checkbox("Loop", &component.Loop);
				ImGui::Checkbox("Play On Start", &component.PlayOnStart);
			});

		// Player Controller Component UI
		ImGuiLibrary::DrawComponent<PlayerControllerComponent>("Player Controller", entity, [](auto& component)
			{
				ImGui::DragFloat("Move Force", &component.MoveForce, 0.1f, 0.0f, 1000.0f);
			});
	}
}
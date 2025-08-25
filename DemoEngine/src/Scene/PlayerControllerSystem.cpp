#include "DemoEngine_PCH.h"
#include "PlayerControllerSystem.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Core/Input.h"
#include <GLFW/glfw3.h>
#include <box2d/box2d.h> 

namespace DemoEngine {

	void UpdatePlayerControllers(Scene& scene, float deltaTime)
	{
		auto view = scene.m_Registry.view<PlayerControllerComponent, Rigidbody2DComponent>();

		for (auto entity : view)
		{
			auto& controller = scene.m_Registry.get<PlayerControllerComponent>(entity);
			auto& rb = scene.m_Registry.get<Rigidbody2DComponent>(entity);

			if (!b2Body_IsValid(rb.RuntimeBody))
				continue;

			glm::vec2 inputDirection = { 0.0f, 0.0f };

			if (Input::IsKeyPressed(GLFW_KEY_W) || Input::IsKeyPressed(GLFW_KEY_UP))
				inputDirection.y += 1.0f;
			if (Input::IsKeyPressed(GLFW_KEY_S) || Input::IsKeyPressed(GLFW_KEY_DOWN))
				inputDirection.y -= 1.0f;
			if (Input::IsKeyPressed(GLFW_KEY_A) || Input::IsKeyPressed(GLFW_KEY_LEFT))
				inputDirection.x -= 1.0f;
			if (Input::IsKeyPressed(GLFW_KEY_D) || Input::IsKeyPressed(GLFW_KEY_RIGHT))
				inputDirection.x += 1.0f;

			if (glm::length(inputDirection) > 0.0f)
			{
				inputDirection = glm::normalize(inputDirection);
				glm::vec2 force = inputDirection * controller.MoveForce;


				b2Vec2 b2Force = { force.x, force.y };
				b2Body_ApplyForceToCenter(rb.RuntimeBody, b2Force, true);
			}
			
		}
	}
}

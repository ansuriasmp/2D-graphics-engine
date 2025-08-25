#pragma once

#include "Renderer/Camera/Camera.h"
#include "Renderer/Camera/EditorCamera.h"

#include "Scene/Components.h"

namespace DemoEngine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const glm::vec4& color);
		static void BeginScene(const EditorCamera& camera);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void Flush();

		//Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4 color,int entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);


		//Circles
		static void DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		//Components
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; };
			uint32_t GetTotalIndexCount() { return QuadCount * 6; };
		};

		static void ResetStats();
		static Statistics GetStats();

		static void DrawBoxCollider(const glm::mat4& transform, const glm::vec4& color, int entityID);
		static void DrawCircleCollider(const glm::mat4& transform, const glm::vec4& color, int entityID);


	private:
		static void StartBatch(); static void NextBatch();
		static void RenderColliderDebug();
	};
}

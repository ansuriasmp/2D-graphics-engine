#include "DemoEngine_PCH.h" 
#include "Renderer2D.h"

#include "Renderer/Data/VertexArray.h" 
#include "Renderer/Shader/Shader.h"
#include "Renderer/Data/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer2DData.h"
#include <glad/glad.h>

namespace DemoEngine
{
	static Renderer2DData s_Data; // Global static instance holding rendering state

	// Initializes all buffers, shaders and state for 2D rendering
	void Renderer2D::Init()
	{
		// Quad buffers and setup
		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float2, "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		// Generate indices for quads
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circle buffer setup (uses same index buffer)
		s_Data.CircleVertexArray = VertexArray::Create();
		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float,  "a_Thickness" },
			{ ShaderDataType::Float,  "a_Fade" },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		// Box collider rendering buffers
		s_Data.BoxColliderVertexArray = VertexArray::Create();
		s_Data.BoxColliderVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(ColliderVertex));
		s_Data.BoxColliderVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.BoxColliderVertexArray->AddVertexBuffer(s_Data.BoxColliderVertexBuffer);
		s_Data.BoxColliderVertexArray->SetIndexBuffer(quadIB);
		s_Data.BoxColliderVertexBufferBase = new ColliderVertex[s_Data.MaxVertices];

		// Circle collider rendering buffers
		s_Data.CircleColliderVertexArray = VertexArray::Create();
		s_Data.CircleColliderVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(ColliderVertex));
		s_Data.CircleColliderVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.CircleColliderVertexArray->AddVertexBuffer(s_Data.CircleColliderVertexBuffer);
		s_Data.CircleColliderVertexArray->SetIndexBuffer(quadIB);
		s_Data.CircleColliderVertexBufferBase = new ColliderVertex[s_Data.MaxVertices];

		// Load shaders
		s_Data.QuadShader = CreateRef<Shader>("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.CircleShader = CreateRef<Shader>("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.BoxColliderShader = CreateRef<Shader>("assets/shaders/BoxColliderShader.glsl");
		s_Data.CircleColliderShader = CreateRef<Shader>("assets/shaders/CircleColliderShader.glsl");

		// Set default quad positions (unit quad)
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		// Create uniform buffer for camera matrices
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		glEnable(GL_DEPTH_TEST); // Enable depth testing
	}

	// Frees dynamically allocated vertex buffer
	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
	}

	// Clears color and depth buffer
	void Renderer2D::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Sets the clear color used on each frame
	void Renderer2D::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	// Begins rendering a scene from a given camera
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
		StartBatch();
	}

	// Begins scene rendering with an editor camera
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
		StartBatch();
	}

	// Ends scene rendering and flushes draw calls
	void Renderer2D::EndScene()
	{
		Flush();
		RenderColliderDebug(); // Optional debug rendering
	}

	// Resets batch buffer pointers and counters
	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.BoxColliderIndexCount = 0;
		s_Data.BoxColliderVertexBufferPtr = s_Data.BoxColliderVertexBufferBase;

		s_Data.CircleColliderIndexCount = 0;
		s_Data.CircleColliderVertexBufferPtr = s_Data.CircleColliderVertexBufferBase;
	}

	// Submits current batch to GPU
	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
			s_Data.QuadShader->Bind();
			s_Data.QuadVertexArray->Bind();
			glDrawElements(GL_TRIANGLES, s_Data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);
			s_Data.CircleShader->Bind();
			s_Data.CircleVertexArray->Bind();
			glDrawElements(GL_TRIANGLES, s_Data.CircleIndexCount, GL_UNSIGNED_INT, nullptr);
			s_Data.Stats.DrawCalls++;
		}
	}

	// Renders wireframes for colliders
	void Renderer2D::RenderColliderDebug()
	{
		if (s_Data.BoxColliderIndexCount == 0 && s_Data.CircleColliderIndexCount == 0)
			return;

		glDisable(GL_DEPTH_TEST);
		glLineWidth(3.0f);

		if (s_Data.BoxColliderIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.BoxColliderVertexBufferPtr - (uint8_t*)s_Data.BoxColliderVertexBufferBase);
			s_Data.BoxColliderVertexBuffer->SetData(s_Data.BoxColliderVertexBufferBase, dataSize);
			s_Data.CircleColliderShader->Bind();
			s_Data.BoxColliderShader->SetMat4("u_ViewProjection", s_Data.CameraBuffer.ViewProjection);
			s_Data.BoxColliderVertexArray->Bind();

			for (uint32_t i = 0; i < s_Data.BoxColliderIndexCount; i += 4)
				glDrawArrays(GL_LINE_LOOP, i, 4);
		}

		if (s_Data.CircleColliderIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleColliderVertexBufferPtr - (uint8_t*)s_Data.CircleColliderVertexBufferBase);
			s_Data.CircleColliderVertexBuffer->SetData(s_Data.CircleColliderVertexBufferBase, dataSize);
			s_Data.CircleColliderShader->Bind();
			s_Data.CircleColliderShader->SetMat4("u_ViewProjection", s_Data.CameraBuffer.ViewProjection);
			s_Data.CircleColliderVertexArray->Bind();

			constexpr int circleSegments = 32;
			for (uint32_t i = 0; i < s_Data.CircleColliderIndexCount; i += circleSegments)
				glDrawArrays(GL_LINE_LOOP, i, circleSegments);
		}

		glLineWidth(1.0f);
		glEnable(GL_DEPTH_TEST);
	}

	// Flushes and resets the current batch
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	// Various DrawQuad overloads
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		DrawQuad(transform, color);
	}

	// Core quad drawing function
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4 color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f }
		};
		const glm::vec2 tilingFactor = { 1.0f, 1.0f };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	// Draw a rotated quad
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, zRotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(zRotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, color);
	}

	// Draws a filled circle
	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness, float fade, int entityID)
	{
		if (s_Data.CircleIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < 4; i++) {
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	// Draws box collider wireframe
	void Renderer2D::DrawBoxCollider(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (s_Data.BoxColliderIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		glm::vec4 corners[4] = {
			{-0.5f, -0.5f, 0.0f, 1.0f},
			{ 0.5f, -0.5f, 0.0f, 1.0f},
			{ 0.5f,  0.5f, 0.0f, 1.0f},
			{-0.5f,  0.5f, 0.0f, 1.0f},
		};

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.BoxColliderVertexBufferPtr->Position = transform * corners[i];
			s_Data.BoxColliderVertexBufferPtr->Color = color;
			s_Data.BoxColliderVertexBufferPtr->EntityID = entityID;
			s_Data.BoxColliderVertexBufferPtr++;
		}

		s_Data.BoxColliderIndexCount += 4;
	}

	// Draws a circle collider wireframe
	void Renderer2D::DrawCircleCollider(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (s_Data.CircleColliderIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		constexpr int segments = 32;
		for (int i = 0; i < segments; i++)
		{
			float angle = (float)i / segments * 2.0f * glm::pi<float>();
			glm::vec4 point = { cos(angle) * 0.5f, sin(angle) * 0.5f, 0.0f, 1.0f };
			s_Data.CircleColliderVertexBufferPtr->Position = transform * point;
			s_Data.CircleColliderVertexBufferPtr->Color = color;
			s_Data.CircleColliderVertexBufferPtr->EntityID = entityID;
			s_Data.CircleColliderVertexBufferPtr++;
		}

		s_Data.CircleColliderIndexCount += segments;
	}

	// Draws a sprite using a sprite renderer component
	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID) {
		DrawQuad(transform, src.Colour, entityID);
	}

	// Resets draw call and quad counters
	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	// Returns current rendering statistics
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}
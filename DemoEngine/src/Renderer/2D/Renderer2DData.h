#pragma once
#include "Renderer2D.h"

#include "Renderer/Data/VertexArray.h" 
#include "Renderer/Shader/Shader.h"
#include "Renderer/Data/UniformBuffer.h"

#include "Renderer/Data/Primatives/QuadVertex.h"
#include "Renderer/Data/Primatives/CircleVertex.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DemoEngine
{
	struct ColliderVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityID;
	};


	struct Renderer2DData
	{
		//Exceeding the max, will trigger a draw call and flush 
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		//Quads
		Ref<VertexArray> QuadVertexArray; 
		Ref<VertexBuffer> QuadVertexBuffer; 
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		glm::vec4 QuadVertexPositions[4];


		//Circles
		Ref<VertexArray> CircleVertexArray; 
		Ref<VertexBuffer> CircleVertexBuffer; 
		Ref<Shader> CircleShader;
		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr; 
		CircleVertex* CircleVertexBufferPtr = nullptr;

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		// Box Colliders
		Ref<VertexArray> BoxColliderVertexArray;
		Ref<VertexBuffer> BoxColliderVertexBuffer;
		uint32_t BoxColliderIndexCount = 0;
		ColliderVertex* BoxColliderVertexBufferBase = nullptr;
		ColliderVertex* BoxColliderVertexBufferPtr = nullptr;
		Ref<Shader> BoxColliderShader;

		// Circle Colliders
		Ref<VertexArray> CircleColliderVertexArray;
		Ref<VertexBuffer> CircleColliderVertexBuffer;
		uint32_t CircleColliderIndexCount = 0;
		ColliderVertex* CircleColliderVertexBufferBase = nullptr;
		ColliderVertex* CircleColliderVertexBufferPtr = nullptr;
		Ref<Shader> CircleColliderShader;

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};
}

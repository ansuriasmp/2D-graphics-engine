#pragma once

#include <glad/glad.h>

namespace DemoEngine
{
	class Renderer3D
	{
	public:
		Renderer3D() {};
		~Renderer3D() = default;

		void PreProcessing();

		//Temporary Example Code 
		void RenderTriangle();
		void SetupTriangle();
		void UpdateViewportSize(int width, int height);

	private:
		//These are used for the demo triangle rendering only 
		uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;

	};
}
	
#include "DemoEngine_PCH.h" 
#include "UniformBuffer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace DemoEngine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		return CreateRef<OpenGLUniformBuffer>(size, binding);
	}
}
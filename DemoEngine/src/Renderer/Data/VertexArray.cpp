#include "DemoEngine_PCH.h" 
#include "VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace DemoEngine
{
	Ref<VertexArray> VertexArray::Create()
	{
		return CreateRef<OpenGLVertexArray>();
	}
}
	
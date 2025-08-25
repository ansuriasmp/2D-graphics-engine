#pragma once

#include <glm/glm.hpp>

namespace DemoEngine
{
	// Vertex utilizado para representar colisionadores (líneas)
	struct ColliderVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityID;
	};
} // namespace DemoEngine

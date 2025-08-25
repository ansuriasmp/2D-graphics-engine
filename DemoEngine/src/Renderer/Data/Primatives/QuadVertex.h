#pragma once
#include "DemoEngine_PCH.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DemoEngine
{
	struct QuadVertex
	{
		//The order of these and the setting of the buffer layout needs to match
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		glm::vec2 TilingFactor;

		//Editor-Only
		int EntityID;
	};
}

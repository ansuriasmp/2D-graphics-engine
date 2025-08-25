
#pragma once
#include "DemoEngine_PCH.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DemoEngine
{
	struct CircleVertex
	{
		glm::vec3 WorldPosition; 
		glm::vec3 LocalPosition; 
		glm::vec4 Color;
		float Thickness;
		float Fade;

		//Editor-Only
		int EntityID;
	};
}

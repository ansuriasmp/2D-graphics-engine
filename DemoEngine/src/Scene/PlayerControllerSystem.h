#pragma once

namespace DemoEngine {
	class Scene;

	// Sistema que mueve entidades con PlayerControllerComponent
	void UpdatePlayerControllers(Scene& scene, float deltaTime);
}

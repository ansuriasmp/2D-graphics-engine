#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "Core/UUID.h"
#include "SceneCamera.h"
#include <box2d/box2d.h>
#include <soloud.h>
#include <soloud_wav.h>

namespace DemoEngine
{

	// Component that assigns a unique identifier to an entity
	struct IDComponent
	{
		UUID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	// Component used to label entities with a name or tag
	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_) : Tag(tag_) {}
	};

	// Component for position, rotation, and scale in 3D space
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}

		// Returns transformation matrix combining translation, rotation, and scale
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation) * rotation *
				glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	// Component for a camera attached to an entity
	struct CameraComponent
	{
		SceneCamera camera;
		bool Primary = true; // Marks it as the main camera
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(ProjectionType t) {
			camera.SetProjectionType(t);
		}

		// Implicit conversion to SceneCamera reference
		operator SceneCamera& () { return camera; }
		operator const SceneCamera& () const { return camera; }
	};

	// Component that defines color for rendering 2D sprites
	struct SpriteRendererComponent
	{
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& colour) : Colour(colour) {}
	};

	// Component to render circles with various visual properties
	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Radius = 0.5f;
		float Thickness = 1.0f;
		float Fade = 0.005f;
		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	// Defines 2D body types for physics simulation
	enum class BodyType2D
	{
		Static = 0,   // Doesn't move
		Dynamic,      // Fully simulated by physics
		Kinematic     // Moves with custom logic but interacts with physics
	};

	// Component that defines physics body properties for 2D rigid bodies
	struct Rigidbody2DComponent
	{
		b2BodyType BodyType = b2BodyType::b2_staticBody;
		bool FixedRotation = false;
		bool AffectedbyGravity = true; // Affected by gravity
		b2MassData Mass;

		// Runtime only: stores the Box2D runtime body ID
		b2BodyId RuntimeBody = b2_nullBodyId;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	// Component for a rectangular collider in 2D physics
	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 HalfSize = { 0.5f, 0.5f }; // Half-extents of the box

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f; // Bounciness

		b2ShapeDef ShapeDef;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	// Component for a circular collider in 2D physics
	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f }; // Offset relative to entity center
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;

		b2ShapeDef ShapeDef;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	// Component that handles audio playback for an entity
	struct AudioComponent
	{
		std::string FilePath;
		SoLoud::Wav Sound;
		bool Loop = false;
		bool PlayOnStart = true;

		// Runtime fields
		int Handle = 0;
		bool IsLoaded = false;

		AudioComponent() = default;
		AudioComponent(const AudioComponent&) = default;
		AudioComponent(const std::string& path, bool playOnStart = false, bool loop = false)
			: FilePath(path), PlayOnStart(playOnStart), Loop(loop)
		{
		}
	};

	// Component that allows an entity to be controlled like a player character
	struct PlayerControllerComponent
	{
		float MoveForce = 10.0f; // Force applied to move the entity

		PlayerControllerComponent() = default;
		PlayerControllerComponent(const PlayerControllerComponent&) = default;
		PlayerControllerComponent(float moveForce)
			: MoveForce(moveForce)
		{
		}
	};

};
#include "DemoEngine_PCH.h" 
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include "Renderer/2D/Renderer2D.h"
#include "Networking/NetStructs.h"
#include "PlayerControllerSystem.h"

namespace DemoEngine
{
	Scene::Scene(const std::string& name, bool isEditorScene)
		: m_Name(name), m_IsEditorScene(isEditorScene)
	{
		// Register handler for CameraComponent
		RegisterComponentHandler<CameraComponent>([](Entity entity, CameraComponent& component) {
			LOG_INFO("Camera component added");
			if (auto* scenePtr = entity.GetScene())
			{
				component.camera.SetViewportSize(scenePtr->GetViewportWidth(), scenePtr->GetViewportHeight());
			}
			});
	}

	Scene::~Scene() {
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		CopyTo(other, newScene);
		return newScene;
	}

	void Scene::CopyTo(Ref<Scene> source, Ref<Scene> destination)
	{
		if (destination == nullptr) destination = CreateRef<Scene>();

		// Copy viewport and scene metadata
		destination->m_ViewportHeight = source->m_ViewportHeight;
		destination->m_ViewportWidth = source->m_ViewportWidth;
		destination->m_SceneID = source->m_SceneID;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = source->m_Registry;
		auto& dstSceneRegistry = destination->m_Registry;

		auto idView = srcSceneRegistry.view<IDComponent>();

		// Recreate entities with same UUID and tag
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& tag = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity en = destination->CreateEntityWithID(uuid, tag);

			enttMap[uuid] = en.m_EntityHandle;
		}

		// Copy supported components
		CopyComponent<TagComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<AudioComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<PlayerControllerComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		destination->m_IsEditorScene = false;
		destination->m_ShouldConnectToServer = source->m_ShouldConnectToServer;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithID(UUID(), name);
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		// Create entity and add default components
		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		// Create new entity with same tag
		Entity newEntity;
		if (entity.HasComponent<TagComponent>())
		{
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		}
		else
		{
			newEntity = CreateEntity();
		}

		// Copy all existing components from the source entity
		CopyComponentIfExists<TagComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<AudioComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<PlayerControllerComponent>(newEntity.m_EntityHandle, m_Registry, entity);

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprite renderers
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Draw circle renderers
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		// Optionally draw colliders
		if (m_ShowColliders)
		{
			{
				// Box collider outlines
				auto view = m_Registry.view<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transform, boxCollider] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::mat4 colliderTransform = transform.GetTransform();

					// Apply offset scaled by transform
					glm::vec3 offset = { boxCollider.Offset.x * transform.Scale.x, boxCollider.Offset.y * transform.Scale.y, 0.0f };
					colliderTransform = glm::translate(colliderTransform, offset);

					Renderer2D::DrawBoxCollider(colliderTransform, { 0.0f, 1.0f, 0.0f, 1.0f }, (int)entity);
				}
			}

			// Circle collider outlines
			{
				auto view = m_Registry.view<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transform, circleCollider] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::mat4 colliderTransform = transform.GetTransform();

					glm::vec3 offset = { circleCollider.Offset.x * transform.Scale.x, circleCollider.Offset.y * transform.Scale.y, 0.0f };
					colliderTransform = glm::translate(colliderTransform, offset);

					Renderer2D::DrawCircleCollider(colliderTransform, { 0.0f, 1.0f, 0.0f, 1.0f }, (int)entity);
				}
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::ConectarENet() {
		if (enet_initialize() != 0) {
			LOG_ERROR("Could not initialize ENet");
			return;
		}
		if (!m_Client)
			m_Client = enet_host_create(nullptr, 1, 2, 0, 0);

		ENetAddress address;
		enet_address_set_host(&address, "localhost");
		address.port = 1234;

		m_Peer = enet_host_connect(m_Client, &address, 2, 0);
		LOG_INFO("Trying to connect to server...");
	}

	Entity Scene::FindOrCreateNetworkEntity(int id)
	{
		std::string name = "Remote_" + std::to_string(id);
		for (auto e : m_Registry.view<TagComponent>())
		{
			auto& tag = m_Registry.get<TagComponent>(e);
			if (tag.Tag == name)
				return Entity{ e, this };
		}
		Entity newEntity = CreateEntity(name);
		newEntity.AddComponent<SpriteRendererComponent>().Colour = glm::vec4(0, 1, 1, 1);
		return newEntity;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Initialize connection if needed
		if (m_ShouldConnectToServer && m_Client == nullptr) {
			LOG_INFO("Connecting...");
			ConectarENet();
		}

		// Handle ENet events
		if (m_ShouldConnectToServer && m_Client && m_Peer) {
			ENetEvent netEvent;
			while (enet_host_service(m_Client, &netEvent, 0) > 0)
			{
				switch (netEvent.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					LOG_INFO("Connected to server.");
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					LOG_INFO("Disconnected from server.");
					break;

				default:
					break;
				}
			}
		}

		// Update systems
		UpdatePlayerControllers(*this, ts.GetSeconds());

		// Step physics simulation
		int subStepCount = 4;
		b2World_Step(m_PhysicsWorld, ts, subStepCount);

		// Check collisions
		b2ContactEvents contactEvents = b2World_GetContactEvents(m_PhysicsWorld);
		for (int i = 0; i < contactEvents.beginCount; i++)
		{
			const auto& beginEvent = contactEvents.beginEvents[i];

			auto bodyA = b2Shape_GetBody(beginEvent.shapeIdA);
			auto bodyB = b2Shape_GetBody(beginEvent.shapeIdB);

			entt::entity entityA = (entt::entity)(uintptr_t)b2Body_GetUserData(bodyA);
			entt::entity entityB = (entt::entity)(uintptr_t)b2Body_GetUserData(bodyB);

			if (m_Registry.valid(entityA) && m_Registry.valid(entityB))
			{
				const auto& tagA = m_Registry.get<TagComponent>(entityA).Tag;
				const auto& tagB = m_Registry.get<TagComponent>(entityB).Tag;

				LOG_INFO("[COLLISION] '{}' collided with '{}'", tagA, tagB);
			}
		}

		// Sync physics to transform
		auto physicsView = m_Registry.view<TransformComponent, Rigidbody2DComponent>();
		for (auto entityID : physicsView)
		{
			Entity entity = { entityID, this };
			auto& transform = m_Registry.get<TransformComponent>(entity);
			auto& rb2d = physicsView.get<Rigidbody2DComponent>(entity);

			b2Transform bodyTransform = b2Body_GetTransform(rb2d.RuntimeBody);

			transform.Translation = { b2Body_GetPosition(rb2d.RuntimeBody).x, b2Body_GetPosition(rb2d.RuntimeBody).y ,0.0f };
			transform.Rotation.z = b2Rot_GetAngle(b2Body_GetRotation(rb2d.RuntimeBody));
		}

		// Find main camera
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		// Render the scene
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.FixedAspectRatio == false)
			{
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::OnRuntimeStart()
	{
		if (m_ShouldConnectToServer && m_Client == nullptr)
		{
			LOG_INFO("Connecting to ENet server from OnRuntimeStart...");
			ConectarENet();
		}

		// Initialize audio
		m_Soloud.init();
		m_Soloud.setGlobalVolume(1.0f);

		auto audioView = m_Registry.view<AudioComponent>();
		for (auto entity : audioView)
		{
			auto& audio = m_Registry.get<AudioComponent>(entity);

			if (!audio.IsLoaded && !audio.FilePath.empty())
			{
				auto result = audio.Sound.load(audio.FilePath.c_str());
				std::cout << "[Audio] Trying to load: " << audio.FilePath << "\n";
				if (result != SoLoud::SO_NO_ERROR)
				{
					std::cout << "[Audio] ERROR loading sound: " << result << "\n";
				}
				else
				{
					std::cout << "[Audio] Sound loaded OK\n";
				}

				audio.Sound.setLooping(audio.Loop);
				audio.IsLoaded = true;
			}

			if (audio.PlayOnStart)
			{
				audio.Handle = m_Soloud.play(audio.Sound);
			}
		}

		// Initialize physics world
		m_WorldDefinition = b2DefaultWorldDef();
		m_WorldDefinition.gravity = { 0.0f, -9.8f };
		m_PhysicsWorld = b2CreateWorld(&m_WorldDefinition);

		// Create physical bodies for all rigidbodies
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto entity : view)
		{
			Entity e = { entity, this };
			auto& rigidbody = m_Registry.get<Rigidbody2DComponent>(entity);
			auto& transform = m_Registry.get<TransformComponent>(entity);

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = rigidbody.BodyType;
			bodyDef.position = { transform.Translation.x, transform.Translation.y };
			bodyDef.fixedRotation = rigidbody.FixedRotation;
			bodyDef.rotation = b2MakeRot(transform.Rotation.z);

			rigidbody.RuntimeBody = b2CreateBody(m_PhysicsWorld, &bodyDef);
			b2Body_SetGravityScale(rigidbody.RuntimeBody, rigidbody.AffectedbyGravity ? 1.0f : 0.0f);

			// Set mass properties
			b2MassData myMassData;
			myMassData.mass = rigidbody.Mass.mass;
			myMassData.center = rigidbody.Mass.center;
			myMassData.rotationalInertia = rigidbody.Mass.rotationalInertia;
			b2Body_SetMassData(rigidbody.RuntimeBody, myMassData);

			// Add box collider shape
			if (e.HasComponent<BoxCollider2DComponent>())
			{
				auto& boxCollider = m_Registry.get<BoxCollider2DComponent>(entity);

				float halfWidth = boxCollider.HalfSize.x * transform.Scale.x;
				float halfHeight = boxCollider.HalfSize.y * transform.Scale.y;

				b2Vec2 offset = { boxCollider.Offset.x * transform.Scale.x, boxCollider.Offset.y * transform.Scale.y };

				b2Polygon boxPoly;
				boxPoly.count = 4;
				boxPoly.vertices[0] = { -halfWidth + offset.x, -halfHeight + offset.y };
				boxPoly.vertices[1] = { halfWidth + offset.x, -halfHeight + offset.y };
				boxPoly.vertices[2] = { halfWidth + offset.x,  halfHeight + offset.y };
				boxPoly.vertices[3] = { -halfWidth + offset.x,  halfHeight + offset.y };

				boxPoly.normals[0] = { 0.0f, -1.0f };
				boxPoly.normals[1] = { 1.0f,  0.0f };
				boxPoly.normals[2] = { 0.0f,  1.0f };
				boxPoly.normals[3] = { -1.0f, 0.0f };
				boxPoly.radius = 0.0f;

				boxCollider.ShapeDef = b2DefaultShapeDef();
				boxCollider.ShapeDef.density = boxCollider.Density;
				boxCollider.ShapeDef.friction = boxCollider.Friction;
				boxCollider.ShapeDef.restitution = boxCollider.Restitution;
				boxCollider.ShapeDef.enableContactEvents = true;

				b2CreatePolygonShape(rigidbody.RuntimeBody, &boxCollider.ShapeDef, &boxPoly);
			}

			// Add circle collider shape
			if (e.HasComponent<CircleCollider2DComponent>())
			{
				auto& circleCollider = m_Registry.get<CircleCollider2DComponent>(entity);

				b2Circle circleShape = {};
				float maxScale = std::max(transform.Scale.x, transform.Scale.y);
				circleShape.radius = circleCollider.Radius * transform.Scale.x;

				// Calculate center using transform scale
				circleShape.center = b2Vec2{
					circleCollider.Offset.x * transform.Scale.x,
					circleCollider.Offset.y * transform.Scale.y
				};

				circleCollider.ShapeDef = b2DefaultShapeDef();
				circleCollider.ShapeDef.density = circleCollider.Density;
				circleCollider.ShapeDef.friction = circleCollider.Friction;
				circleCollider.ShapeDef.restitution = circleCollider.Restitution;
				circleCollider.ShapeDef.enableContactEvents = true;

				b2CreateCircleShape(rigidbody.RuntimeBody, &circleCollider.ShapeDef, &circleShape);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		// 1. Clear runtime body references from all rigidbodies
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto entityID : view)
		{
			auto& rb2d = m_Registry.get<Rigidbody2DComponent>(entityID);
			rb2d.RuntimeBody = b2_nullBodyId; // prevent dangling references
		}

		// 2. Destroy the physics world if valid
		if (b2World_IsValid(m_PhysicsWorld))
			b2DestroyWorld(m_PhysicsWorld);

		// 3. Reset global world ID
		m_PhysicsWorld = b2_nullWorldId;

		// Shut down audio
		m_Soloud.deinit();

		// Disconnect from ENet server if connected
		if (m_Client) {
			enet_peer_disconnect_now(m_Peer, 0);
			enet_host_destroy(m_Client);
			m_Client = nullptr;
			m_Peer = nullptr;
		}

		// Cleanup ENet
		enet_deinitialize();

		// Reset the connect-to-server flag so checkbox is unchecked after stopping runtime
		m_ShouldConnectToServer = false;
	}
}
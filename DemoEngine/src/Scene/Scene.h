#pragma once
#include "Core/Timestep.h"
#include "entt.hpp"
#include "Core/UUID.h"
#include <box2d/box2d.h>
#include "Components.h"
#include <typeindex>

#include "Renderer/Camera/EditorCamera.h"
#include <enet\enet.h>

namespace DemoEngine
{
	class Entity;


	struct CopiedComponent
	{
		std::type_index Type;
		std::function<void(Entity&)> PasteFunction;

		CopiedComponent() : Type(typeid(void)) {} // Default constructor for "empty" state
	};

	class Scene
	{
	public:
		Scene(const std::string& name = "UntitledScene", bool isEditorScene = false);
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);
		static void Scene::CopyTo(Ref<Scene> source, Ref<Scene> destination);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");

		Entity DuplicateEntity(Entity entity);

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);

		void DestroyEntity(Entity entity);
		void OnViewportResize(uint32_t width, uint32_t height);

		void OnRuntimeStart();
		void OnRuntimeStop();

		bool m_ShowColliders = true;

		bool m_ShouldConnectToServer = false;

		void ConectarENet();

		Entity FindOrCreateNetworkEntity(int id);

		inline void SetShowColliders(bool show) { m_ShowColliders = show; }
		inline bool GetShowColliders() const { return m_ShowColliders; }

		template<typename... Components>
		auto GetAllEntitiesWith() 
		{
			return m_Registry.view<Components...>();
		}

		template<typename T>
		void RegisterComponentHandler(std::function<void(Entity, T&)> handler)
		{
			m_ComponentHandlers[typeid(T)] = [handler](Entity entity) {
				handler(entity, entity.GetComponent<T>());
			};
		}

		template<typename T>
		void OnComponentAdded(Entity entity, T& component)
		{
			auto it = m_ComponentHandlers.find(typeid(T));
			if (it != m_ComponentHandlers.end())
			{
				// Call the registered handler if it exists 
				it->second(entity);
			}
			else
			{
				// Default behavior: log the component addition
				LOG_INFO("Component of type {0} added to Entity ID {1}", typeid(T).name(), (uint32_t)entity);
			}
		}


		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			if (m_Registry.any_of<TComponent>(src))
			{
				auto& srcComponent = m_Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}

		template<typename T>
		static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map < UUID, entt::entity>& enttMap)
		{
			auto srcEntities = srcRegistry.view<T>();
			for (auto srcEntity : srcEntities)
			{
				entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);
				auto& srcComponent = srcRegistry.get<T>(srcEntity);
				auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
			}
		}

		template<typename T>
		void CopyComponent(Entity entity)
		{
			if (!entity.HasComponent<T>())
				return;

			auto& component = entity.GetComponent<T>(); 
			m_CopiedComponent.Type = typeid(T);
			m_CopiedComponent.PasteFunction = [component](Entity& targetEntity) mutable { 
				if (!targetEntity.HasComponent<T>())
					targetEntity.AddComponent<T>(component); // Add the copied component to the target entity
				else
					targetEntity.GetComponent<T>() = component; // Update if already exists
				};
		}

		void PasteComponent(Entity & targetEntity)
		{
			if (m_CopiedComponent.Type != typeid(void) && m_CopiedComponent.PasteFunction)
			{
				m_CopiedComponent.PasteFunction(targetEntity); 
				m_CopiedComponent.Type = typeid(void);
			}
		}

		bool HasCopiedComponent() const
		{
			return m_CopiedComponent.Type != typeid(void);
		}

		entt::registry m_Registry;
		
	private:
		uint32_t GetViewportWidth() { return m_ViewportWidth; }
		uint32_t GetViewportHeight() { return m_ViewportHeight; }

		void SetSceneID(UUID id) { m_SceneID = id; }
		UUID GetSceneID() { return m_SceneID; }

		b2WorldDef m_WorldDefinition;

		SoLoud::Soloud m_Soloud;

	private:
		UUID m_SceneID;

		ENetHost* m_Client = nullptr;
		ENetPeer* m_Peer = nullptr;
		int m_ClientIndex = 0;

		//Container for all the components and entities
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneHierarchy;

		std::unordered_map<std::type_index, std::function<void(Entity)>> m_ComponentHandlers;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_Name;
		bool m_IsEditorScene = false;

		CopiedComponent m_CopiedComponent;

		b2WorldId m_PhysicsWorld = b2_nullWorldId;
	};

}

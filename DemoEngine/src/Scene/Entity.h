#pragma once 
#include "Scene.h"
#include "entt.hpp"

namespace DemoEngine
{
	// Wrapper class around an entt entity for easier component management
	class Entity
	{
	public:
		Entity() = default; // Default constructor (null entity)
		Entity(entt::entity handle_, Scene* scene_); // Constructor with entity handle and scene pointer
		Entity(const Entity& other) = default; // Copy constructor

		// Adds a new component of type T to this entity
		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			CORE_ASSERT(!HasComponent<T>(), "Entity already has component");

			// Create and add the component to the registry
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component); // Call OnComponentAdded for potential custom logic
			return component;
		}

		// Retrieves a reference to component T
		template <typename T>
		T& GetComponent()
		{
			CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		// Const version of GetComponent
		template<typename T>
		T& GetComponent() const
		{
			CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		// Checks if this entity has a component of type T
		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		// Const version of HasComponent
		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		// Removes a component of type T from this entity
		template<typename T>
		void RemoveComponent()
		{
			CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		// Conversion operator to uint32_t (useful for IDs)
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		// Conversion operator to entt::entity
		operator entt::entity() const { return m_EntityHandle; }

		// Checks if the entity is valid (not null)
		operator bool() const { return m_EntityHandle != entt::null; }

		// Compares two entities for equality (same handle and scene)
		bool operator ==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }

		// Inequality operator
		bool operator !=(const Entity& other) const { return !(*this == other); }

		// Returns a pointer to the scene this entity belongs to
		Scene* GetScene() { return m_Scene; }

		// Returns this entity's UUID
		UUID GetUUID() const { return GetComponent<IDComponent>().ID; }

		// Returns the UUID of the scene this entity is in
		UUID GetSceneUUID() const;

	private:

		// Handle to the actual entt entity
		entt::entity m_EntityHandle = entt::null;

		// Pointer to the scene that owns this entity
		Scene* m_Scene = nullptr;

		// Grant Scene and SceneSerialiser access to private members
		friend class Scene;
		friend class SceneSerialiser;
	};
}
#include "DemoEngine_PCH.h" 
#include "SceneSerialiser.h" 
#include "Entity.h"
#include "Components.h"
#include "Utils/YamlConverter.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace DemoEngine
{
	SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	// Adaptaciones para los nuevos structs de Rigidbody2DComponent, BoxCollider2DComponent y CircleCollider2DComponent

	static void SerialiseAudioComponent(YAML::Emitter& out, Entity entity)
	{
		auto& audio = entity.GetComponent<AudioComponent>();
		out << YAML::Key << "AudioComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "FilePath" << YAML::Value << audio.FilePath;
		out << YAML::Key << "Loop" << YAML::Value << audio.Loop;
		out << YAML::Key << "PlayOnStart" << YAML::Value << audio.PlayOnStart;
		out << YAML::EndMap;
	}

	static void DeserialiseAudioComponent(Entity& entity, const YAML::Node& node)
	{
		auto& audio = entity.AddComponent<AudioComponent>();
		audio.FilePath = node["FilePath"] ? node["FilePath"].as<std::string>() : "";
		audio.Loop = node["Loop"] ? node["Loop"].as<bool>() : false;
		audio.PlayOnStart = node["PlayOnStart"] ? node["PlayOnStart"].as<bool>() : true;
	}

	static void SerialiseRigidbody2D(YAML::Emitter& out, Entity entity)
	{
		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		out << YAML::Key << "Rigidbody2DComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "BodyType" << YAML::Value << (int)rb.BodyType;
		out << YAML::Key << "FixedRotation" << YAML::Value << rb.FixedRotation;
		out << YAML::Key << "AffectedbyGravity" << YAML::Value << rb.AffectedbyGravity;
		out << YAML::Key << "Mass" << YAML::Value << rb.Mass.mass;
		glm::vec2 MassCenterTmp = { rb.Mass.center.x, rb.Mass.center.y };
		out << YAML::Key << "CenterOfMass" << YAML::Value << MassCenterTmp;
		out << YAML::Key << "Inertia" << YAML::Value << rb.Mass.rotationalInertia;
		out << YAML::EndMap;
	}

	static void DeserialiseRigidbody2D(Entity& entity, const YAML::Node& rbNode)
	{
		auto& rb2d = entity.AddComponent<Rigidbody2DComponent>();
		rb2d.BodyType = rbNode["BodyType"] ? (b2BodyType)rbNode["BodyType"].as<int>() : b2_staticBody;
		rb2d.FixedRotation = rbNode["FixedRotation"] ? rbNode["FixedRotation"].as<bool>() : false;
		rb2d.AffectedbyGravity = rbNode["AffectedbyGravity"] ? rbNode["AffectedbyGravity"].as<bool>() : true;
		rb2d.Mass.mass = rbNode["Mass"] ? glm::max(0.0f, rbNode["Mass"].as<float>()) : 1.0f;
		glm::vec2 MassCenterTmp = { 0.0f, 0.0f };
		MassCenterTmp = rbNode["CenterOfMass"] ? rbNode["CenterOfMass"].as<glm::vec2>() : glm::vec2(0.0f);
		rb2d.Mass.center.x = MassCenterTmp.x;
		rb2d.Mass.center.y = MassCenterTmp.y;
		rb2d.Mass.rotationalInertia = rbNode["Inertia"] ? glm::max(0.0f, rbNode["Inertia"].as<float>()) : 0.0f;
	}

	static void SerialiseBoxCollider2D(YAML::Emitter& out, Entity entity)
	{
		auto& box = entity.GetComponent<BoxCollider2DComponent>();
		out << YAML::Key << "BoxCollider2DComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Offset" << YAML::Value << box.Offset;
		out << YAML::Key << "HalfSize" << YAML::Value << box.HalfSize;
		out << YAML::Key << "Density" << YAML::Value << box.Density;
		out << YAML::Key << "Friction" << YAML::Value << box.Friction;
		out << YAML::Key << "Restitution" << YAML::Value << box.Restitution;
		out << YAML::EndMap;
	}

	static void DeserialiseBoxCollider2D(Entity& entity, const YAML::Node& boxNode)
	{
		auto& box = entity.AddComponent<BoxCollider2DComponent>();
		box.Offset = boxNode["Offset"] ? boxNode["Offset"].as<glm::vec2>() : glm::vec2(0.0f);
		box.HalfSize = boxNode["HalfSize"] ? boxNode["HalfSize"].as<glm::vec2>() : glm::vec2(0.5f);
		box.Density = boxNode["Density"] ? boxNode["Density"].as<float>() : 1.0f;
		box.Friction = boxNode["Friction"] ? boxNode["Friction"].as<float>() : 0.5f;
		box.Restitution = boxNode["Restitution"] ? boxNode["Restitution"].as<float>() : 0.0f;
	}

	static void SerialiseCircleCollider2D(YAML::Emitter& out, Entity entity)
	{
		auto& circle = entity.GetComponent<CircleCollider2DComponent>();
		out << YAML::Key << "CircleCollider2DComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Offset" << YAML::Value << circle.Offset;
		out << YAML::Key << "Radius" << YAML::Value << circle.Radius;
		out << YAML::Key << "Density" << YAML::Value << circle.Density;
		out << YAML::Key << "Friction" << YAML::Value << circle.Friction;
		out << YAML::Key << "Restitution" << YAML::Value << circle.Restitution;
		out << YAML::EndMap;
	}

	static void DeserialiseCircleCollider2D(Entity& entity, const YAML::Node& circleNode)
	{
		auto& circle = entity.AddComponent<CircleCollider2DComponent>();
		circle.Offset = circleNode["Offset"] ? circleNode["Offset"].as<glm::vec2>() : glm::vec2(0.0f);
		circle.Radius = circleNode["Radius"] ? circleNode["Radius"].as<float>() : 0.5f;
		circle.Density = circleNode["Density"] ? circleNode["Density"].as<float>() : 1.0f;
		circle.Friction = circleNode["Friction"] ? circleNode["Friction"].as<float>() : 0.5f;
		circle.Restitution = circleNode["Restitution"] ? circleNode["Restitution"].as<float>() : 0.0f;
	}

	// ------------------- SERIALIZACIÓN DE ENTIDAD -----------------------

	static void SerialiseEntity(YAML::Emitter& out, Entity entity)
	{
		CORE_ASSERT("Entity does not have ID component", entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().Tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			auto& tr = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Translation" << YAML::Value << tr.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tr.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tr.Scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.camera;
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Camera" << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "Perspective FOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNear();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFar();
			out << YAML::EndMap;
			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Colour" << YAML::Value << src.Colour;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			auto& src = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Colour" << YAML::Value << src.Color;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
			SerialiseRigidbody2D(out, entity);

		if (entity.HasComponent<BoxCollider2DComponent>())
			SerialiseBoxCollider2D(out, entity);

		if (entity.HasComponent<CircleCollider2DComponent>())
			SerialiseCircleCollider2D(out, entity);

		if (entity.HasComponent<AudioComponent>())
			SerialiseAudioComponent(out, entity);

		if (entity.HasComponent<PlayerControllerComponent>())
		{
			auto& pc = entity.GetComponent<PlayerControllerComponent>();
			out << YAML::Key << "PlayerControllerComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "MoveForce" << YAML::Value << pc.MoveForce;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	// ------------------- SERIALIZACIÓN ESCENA -----------------------

	void SceneSerialiser::Serialise(const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		auto allEntities = m_Scene->m_Registry.view<entt::entity>();
		for (auto entityID : allEntities)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity) continue;
			SerialiseEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		if (!fout.is_open())
		{
			LOG_ERROR("No se pudo abrir el archivo de escena para escritura: {}", filePath);
			return;
		}
		fout << out.c_str();
		LOG_INFO("Archivo de escena guardado correctamente en '{}'", filePath);
	}

	// ------------------- DESERIALIZACIÓN ESCENA -----------------------

	bool SceneSerialiser::Deserialise(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data;
		try { data = YAML::Load(strStream.str()); }
		catch (YAML::ParserException e)
		{
			LOG_ERROR("YAML parse error: {}", e.what());
			return false;
		}

		if (!data["Scene"]) return false;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name = entity["TagComponent"] ? entity["TagComponent"]["Tag"].as<std::string>() : "";

				Entity deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

				if (entity["TransformComponent"])
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = entity["TransformComponent"]["Translation"].as<glm::vec3>();
					tc.Rotation = entity["TransformComponent"]["Rotation"].as<glm::vec3>();
					tc.Scale = entity["TransformComponent"]["Scale"].as<glm::vec3>();
				}

				if (entity["CameraComponent"])
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = entity["CameraComponent"]["Camera"];
					cc.camera.SetProjectionType((ProjectionType)cameraProps["ProjectionType"].as<int>());
					cc.camera.SetPerspectiveFOV(cameraProps["Perspective FOV"].as<float>());
					cc.camera.SetPerspectiveNear(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.SetPerspectiveFar(cameraProps["PerspectiveFar"].as<float>());
					cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.SetOrthographicNear(cameraProps["OrthographicNear"].as<float>());
					cc.camera.SetOrthographicFar(cameraProps["OrthographicFar"].as<float>());
					cc.Primary = entity["CameraComponent"]["Primary"].as<bool>();
					cc.FixedAspectRatio = entity["CameraComponent"]["FixedAspectRatio"].as<bool>();
				}

				if (entity["SpriteRendererComponent"])
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Colour = entity["SpriteRendererComponent"]["Colour"].as<glm::vec4>();
				}

				if (entity["CircleRendererComponent"])
				{
					auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = entity["CircleRendererComponent"]["Colour"].as<glm::vec4>();
				}

				if (entity["Rigidbody2DComponent"])
					DeserialiseRigidbody2D(deserializedEntity, entity["Rigidbody2DComponent"]);

				if (entity["BoxCollider2DComponent"])
					DeserialiseBoxCollider2D(deserializedEntity, entity["BoxCollider2DComponent"]);

				if (entity["CircleCollider2DComponent"])
					DeserialiseCircleCollider2D(deserializedEntity, entity["CircleCollider2DComponent"]);

				if (entity["AudioComponent"])
					DeserialiseAudioComponent(deserializedEntity, entity["AudioComponent"]);

				if (entity["PlayerControllerComponent"])
				{
					auto& pc = deserializedEntity.AddComponent<PlayerControllerComponent>();
					pc.MoveForce = entity["PlayerControllerComponent"]["MoveForce"].as<float>();
				}
			}
		}
		return true;
	}
}

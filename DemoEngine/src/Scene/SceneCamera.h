#pragma once
#include "Renderer/Camera/Camera.h"

namespace DemoEngine
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNear() const { return m_OrthographicNear; }
		void SetOrthographicNear(float near_) { m_OrthographicNear = near_; RecalculateProjection(); }
		float GetOrthographicFar() const { return m_OrthographicFar; }
		void SetOrthographicFar(float far_) { m_OrthographicFar = far_; RecalculateProjection(); }

		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveNear() const { return m_PerspectiveNear; }
		void SetPerspectiveNear(float near_) { m_PerspectiveNear = near_; RecalculateProjection(); }
		float GetPerspectiveFar() const { return m_PerspectiveFar; }
		void SetPerspectiveFar(float far_) { m_PerspectiveFar = far_; RecalculateProjection(); }

		void SetProjectionType(ProjectionType type)
		{
			m_CameraType = type;
			RecalculateProjection();
		}

		ProjectionType GetProjectionType() { return m_CameraType; }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_CameraType = ProjectionType::Orthographic;

		float m_aspectRatio = 1280.0f / 720.0f;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(60.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;
	};
}

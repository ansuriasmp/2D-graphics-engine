#include "DemoEngine_PCH.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace DemoEngine
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_CameraType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_CameraType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip; 
		m_PerspectiveFar = farClip;
		RecalculateProjection();

	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (width > 0 && height > 0) {
			m_aspectRatio = (float)width / (float)height;
			RecalculateProjection();
		}
		else
		{
			LOG_ERROR("SceneCamera::SetViewportSize: Setting Viewport to Width {0} and Height {1}", width, height);
		}

	}
	
	void SceneCamera::RecalculateProjection()
	{
		if (m_CameraType == ProjectionType::Orthographic)
		{
			float orthoLeft = -m_OrthographicSize * 0.5f * m_aspectRatio;
			float orthoRight = m_OrthographicSize * 0.5f * m_aspectRatio;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
				m_OrthographicNear, m_OrthographicFar);
		}		
		else if (m_CameraType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_aspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}		
	}
}
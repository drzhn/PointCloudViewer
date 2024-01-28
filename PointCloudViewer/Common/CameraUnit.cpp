#include "CameraUnit.h"

#include "Math/MathTypes.h"

#include "Utils/Assert.h"

namespace PointCloudViewer
{
	CameraUnit::CameraUnit(float aspect, float width, float height, float fovDeg, float nearPlane, float farPlane):
		m_type(CameraUnitType::Perspective),
		m_aspect(aspect),
		m_width(width),
		m_height(height),
		m_fovRad(math::toRadians(fovDeg)),
		m_near(nearPlane),
		m_far(farPlane)
	{
	}

	CameraUnit::CameraUnit(float aspect, float size, float nearPlane, float farPlane):
		m_type(CameraUnitType::Orthographic),
		m_aspect(aspect),
		m_size(size),
		m_near(nearPlane),
		m_far(farPlane)
	{
	}

	math::mat4x4 CameraUnit::GetProjMatrix() const
	{
		switch (m_type)
		{
		case CameraUnitType::Perspective:
			return math::perspectiveFovLH_ZO(m_fovRad, m_width, m_height, m_near, m_far);
		case CameraUnitType::Orthographic:
			return math::orthoLH_ZO(-m_size * m_aspect, m_size * m_aspect, -m_size, m_size, m_near, m_far);
		}
		ASSERT(false);
		return math::identity();
	}

	math::mat4x4 CameraUnit::GetViewMatrix(const math::xvec4 position, const math::quat rotation) const
	{
		const math::xvec4 focus = position + math::rotate3(math::xforward, rotation);
		const math::xvec4 up = math::rotate3(math::xup, rotation);

		return math::lookAtLH(position, focus, up);
	}
}

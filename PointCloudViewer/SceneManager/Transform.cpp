#include "Transform.h"

#include "SceneManager/GameObject.h"
#include "Common/Math/MathTypes.h"

namespace PointCloudViewer
{
	Transform::Transform(GameObject& gameObject) :
		Transform(
			gameObject,
			math::vec3(0.f, 0.f, 0.f),
			math::vec3(0.f, 0.f, 0.f),
			math::vec3(1.f, 1.f, 1.f))
	{
	}

	Transform::Transform(GameObject& gameObject, math::vec3 pos, math::vec3 rot, math::vec3 scale):
		m_gameObject(gameObject)
	{
		SetPosition(pos);
		SetRotation(rot);
		SetScale(scale);
	}

	void Transform::SetPosition(const math::vec3 pos) noexcept
	{
		SetXPosition(math::loadPosition(pos));
	}


	void Transform::SetRotation(math::vec3 rot) noexcept
	{
		SetRotation(math::eulerToQuat(math::vec3(
			math::toRadians(rot.x),
			math::toRadians(rot.y),
			math::toRadians(rot.z)
		)));
	}

	void Transform::SetRotation(math::quat rot) noexcept
	{
		m_localRotation = rot;
		UpdateMatrix();
	}

	void Transform::SetScale(const math::vec3 scale) noexcept
	{
		SetXScale(math::loadPosition(scale));
	}

	void Transform::SetXPosition(math::xvec4 pos) noexcept
	{
		m_localPosition = pos;
		UpdateMatrix();
	}

	void Transform::SetXScale(math::xvec4 scale) noexcept
	{
		m_localScale = scale;
		UpdateMatrix();
	}

	void Transform::UpdateMatrix()
	{
		m_trs = math::trs(m_localPosition, m_localRotation, m_localScale);
	}

	const math::mat4x4& Transform::GetModelMatrix() const noexcept
	{
		return m_trs;
	}

	math::vec3 Transform::GetPosition() const noexcept { return math::toVec3(m_localPosition); }

	math::quat Transform::GetRotation() const noexcept { return m_localRotation; }

	math::vec3 Transform::GetScale() const noexcept { return math::toVec3(m_localScale); }

	math::vec3 Transform::GetForward() const noexcept { return math::toVec3(GetXForward()); }

	math::vec3 Transform::GetUp() const noexcept { return math::toVec3(GetXUp()); }

	math::vec3 Transform::GetRight() const noexcept { return math::toVec3(GetXRight()); }

	math::xvec4 Transform::GetXPosition() const noexcept { return m_localPosition; }

	math::xvec4 Transform::GetXScale() const noexcept { return m_localScale; }

	math::xvec4 Transform::GetXForward() const noexcept { return math::mul(GetModelMatrix(), math::xforward); }

	math::xvec4 Transform::GetXUp() const noexcept { return math::mul(GetModelMatrix(), math::xup); }

	math::xvec4 Transform::GetXRight() const noexcept { return math::mul(GetModelMatrix(), math::xright); }
}

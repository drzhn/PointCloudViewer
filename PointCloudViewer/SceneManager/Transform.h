#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Common/Math/MathTypes.h"

namespace PointCloudViewer
{
	class GameObject;

	class Transform
	{
	public:
		Transform() = delete;

		explicit Transform(GameObject& gameObject);
		explicit Transform(GameObject& gameObject, math::vec3 pos, math::vec3 rot, math::vec3 scale);

		void SetRotation(math::vec3 rot) noexcept;
		void SetRotation(math::quat rot) noexcept;

		void SetPosition(math::vec3 pos) noexcept;
		void SetScale(math::vec3 scale) noexcept;

		void SetXPosition(math::xvec4 pos) noexcept;
		void SetXScale(math::xvec4 scale) noexcept;

		[[nodiscard]] math::quat GetRotation() const noexcept;

		[[nodiscard]] math::vec3 GetPosition() const noexcept;
		[[nodiscard]] math::vec3 GetScale() const noexcept;
		[[nodiscard]] math::vec3 GetForward() const noexcept;
		[[nodiscard]] math::vec3 GetUp() const noexcept;
		[[nodiscard]] math::vec3 GetRight() const noexcept;

		[[nodiscard]] math::xvec4 GetXPosition() const noexcept;
		[[nodiscard]] math::xvec4 GetXScale() const noexcept;
		[[nodiscard]] math::xvec4 GetXForward() const noexcept;
		[[nodiscard]] math::xvec4 GetXUp() const noexcept;
		[[nodiscard]] math::xvec4 GetXRight() const noexcept;

		[[nodiscard]] const math::mat4x4& GetModelMatrix() const noexcept;

	private:
		void UpdateMatrix();


		math::xvec4 m_localPosition;
		math::quat m_localRotation;
		math::xvec4 m_localScale;

		math::mat4x4 m_trs;

		// There is no scene object without Transform and no Transform without the scene object
		// I moved all math to separate class just because of nicer code style
		GameObject& m_gameObject;
	};
}

#endif //TRANSFORM_H

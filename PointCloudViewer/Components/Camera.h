#ifndef CAMERA_H
#define CAMERA_H

#include "Component.h"

#include "Common/CameraUnit.h"

namespace PointCloudViewer
{
	class IRenderer;

	class Camera : public Component
	{
	public:
		Camera() = delete;
		explicit Camera(GameObject& go, IRenderer* manager, float cameraNear, float cameraFar, float cameraFov);
		void Enable() override;
		void Disable() override;
		void Update() override;
		[[nodiscard]] math::mat4x4 GetViewMatrix() const;
		[[nodiscard]] math::mat4x4 GetProjMatrix() const;

		[[nodiscard]] float GetFovRadians() const;
		[[nodiscard]] float GetNear() const;
		[[nodiscard]] float GetFar() const;
		[[nodiscard]] float GetAspect() const;
	private:
		CameraUnit m_cameraUnit;
		IRenderer* m_manager;
	};
}

#endif //CAMERA_H

#ifndef CAMERA_BEHAVIOUR_H
#define CAMERA_BEHAVIOUR_H

#include "Components/Component.h"

namespace PointCloudViewer
{
	class CameraBehaviour : public Component
	{
	public :
		explicit CameraBehaviour(GameObject& go);

		void Enable() final;

		void Disable() final;

		void Update() final;

		float m_speed = 8.0;
	};
}

#endif //CAMERA_BEHAVIOUR_H

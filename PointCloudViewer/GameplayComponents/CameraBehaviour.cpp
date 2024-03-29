#include "CameraBehaviour.h"

#include "Common/Time.h"
#include "InputManager/InputManager.h"
#include "SceneManager/GameObject.h"
#include "SceneManager/Transform.h"
#include "Utils/TimeCounter.h"

namespace PointCloudViewer
{
	CameraBehaviour::CameraBehaviour(GameObject& go)
		: Component(go)
	{
	}

	void CameraBehaviour::Enable()
	{
		m_enabled = true;
	}

	void CameraBehaviour::Disable()
	{
		m_enabled = false;
	}

	void CameraBehaviour::Update()
	{
		float deltaTime = Time::GetDeltaTime();

		float deltaX =
#ifdef jmath_FORCE_LEFT_HANDED
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_D) ? deltaTime : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_A) ? deltaTime : 0);
#else
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_D) ? deltaTime : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_A) ? deltaTime : 0);
#endif

		float deltaZ = (InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_W) ? Time::GetDeltaTime() : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_S) ? Time::GetDeltaTime() : 0);

		float deltaY = (InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_K) ? Time::GetDeltaTime() : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_M) ? Time::GetDeltaTime() : 0);

		float deltaYRotation = (InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_E) ? Time::GetDeltaTime() : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_Q) ? Time::GetDeltaTime() : 0);

		float deltaXRotation = (InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_N) ? Time::GetDeltaTime() : 0) -
			(InputManager::Get()->GetKeyDown(KeyCode::KEYCODE_J) ? Time::GetDeltaTime() : 0);


		math::vec3 vec = math::vec3(deltaX, deltaY, deltaZ);

		const math::xvec4 vecWorld = math::rotate3(
			math::loadVec4(math::vec4(deltaX, deltaY, deltaZ, 1.f)),
			m_gameObject.GetTransform().GetRotation()
		);

		m_gameObject.GetTransform().SetXPosition(
			m_gameObject.GetTransform().GetXPosition() + math::mul(m_speed, vecWorld)
		);

		m_gameObject.GetTransform().SetRotation(
			math::mul(m_gameObject.GetTransform().GetRotation(),
			           math::mul(math::angleAxis(m_gameObject.GetTransform().GetXRight(), deltaXRotation),
			                      math::angleAxis(math::xup, deltaYRotation)))
		);
	}
}

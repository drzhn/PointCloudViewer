#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <memory>
#include <string>

#include "SceneManager/Transform.h"

#include "GameObject.h"
#include "Components/Component.h"

namespace PointCloudViewer
{
	class Transform;
	class TransformProvider;

	class GameObject
	{
	public:
		explicit GameObject(const char* name) :
			m_name(name),
			m_transform(*this)
		{
		}

		[[nodiscard]] Transform& GetTransform() noexcept { return m_transform; }

		~GameObject();

		void Update();

		void AddComponent(std::unique_ptr<Component> component);

	protected:
		std::string m_name;
		Transform m_transform;

	private:
		std::vector<std::unique_ptr<Component>> m_components;
	};
}


#endif //GAME_OBJECT_H

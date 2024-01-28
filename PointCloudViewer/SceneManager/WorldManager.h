#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>

#include "GameObject.h"
#include "Common/Singleton.h"
#include "RenderManager/IRenderer.h"

namespace PointCloudViewer
{
	class WorldManager : public Singleton<WorldManager>
	{
	public:
		WorldManager() = delete;

		explicit WorldManager(HWND gameWindowHandle);

		void Init();

		void Start();

		[[nodiscard]] IRenderer& GetRenderer() const noexcept { return *m_renderManager; }

		void Stop();

		void Update();

		~WorldManager();

	private:
		std::unique_ptr<GameObject> m_camera = nullptr;
		std::unique_ptr<IRenderer> m_renderManager;
	};
}

#endif //SCENE_MANAGER_H

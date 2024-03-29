#ifndef JOY_ENGINE_H
#define JOY_ENGINE_H

#include <functional>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace PointCloudViewer
{
	class InputManager;

	class ThreadManager;

	class GraphicsManager;

	class MemoryManager;

	class DataManager;

	class DescriptorManager;

	class WorldManager;

	class EngineDataProvider;

	class IWindowHandler
	{
	public:
		virtual void HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		void SetDeltaTimeHandler(const std::function<void(float)> handler)
		{
			m_deltaTimeHandler = handler;
		}

	protected:
		std::function<void(float)> m_deltaTimeHandler;
	};

	class PointCloudViewer final : public IWindowHandler
	{
	public:
		PointCloudViewer(HWND gameWindowHandle);

		void Init() const noexcept;

		void Start() const noexcept;

		void UpdateTask() const noexcept;

		void Stop() const noexcept;

		~PointCloudViewer();

		void HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	private:
		HWND m_windowHandle;

		std::unique_ptr<InputManager> m_inputManager;
		std::unique_ptr<ThreadManager> m_threadManager;
		std::unique_ptr<GraphicsManager> m_graphicsManager;
		std::unique_ptr<MemoryManager> m_memoryManager;
		std::unique_ptr<DataManager> m_dataManager;
		std::unique_ptr<DescriptorManager> m_descriptorSetManager;
		std::unique_ptr<EngineDataProvider> m_engineData;
		std::unique_ptr<WorldManager> m_worldManager;
	};
}


#endif//JOY_ENGINE_H

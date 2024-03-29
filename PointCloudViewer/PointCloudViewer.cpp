#include "PointCloudViewer.h"

#include <string>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"

#include "SceneManager/WorldManager.h"
#include "EngineDataProvider/EngineDataProvider.h"
#include "MemoryManager/MemoryManager.h"
#include "DataManager/DataManager.h"
#include "DescriptorManager/DescriptorManager.h"
#include "GraphicsManager/GraphicsManager.h"
#include "Common/Time.h"
#include "InputManager/InputManager.h"
#include "ThreadManager/LockFreeFlag.h"
#include "ThreadManager/ThreadManager.h"
#include "Utils/TimeCounter.h"

#ifdef _DEBUG
#include "dxgidebug.h"
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace PointCloudViewer
{
	auto g_startTime = std::chrono::high_resolution_clock::now();

	PointCloudViewer::PointCloudViewer(HWND gameWindowHandle):
		m_threadManager(new ThreadManager()),
		m_inputManager(new InputManager()),
		m_graphicsManager(new GraphicsManager()),
		m_memoryManager(new MemoryManager()),
		m_dataManager(new DataManager()),
		m_descriptorSetManager(new DescriptorManager()),
		m_engineData(new EngineDataProvider()),
		m_worldManager(new WorldManager(gameWindowHandle))
	{
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			//ImGuiIO& io = ImGui::GetIO();
			//(void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			ImGui::StyleColorsDark();
			ImGui_ImplWin32_Init(gameWindowHandle);
		}
	}

	void PointCloudViewer::Init() const noexcept
	{
		Time::Init(m_deltaTimeHandler);

		// creating internal engine materials
		m_engineData->Init();
		// loading scene from disk
		m_worldManager->Init();

		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - g_startTime).count();
		Logger::LogFormat("=========== Context initialized in %.3f seconds ===========\n", time);

		m_memoryManager->PrintStats();
		m_descriptorSetManager->PrintStats();

		Logger::Log("==================================================================\n");
	}

	LockFreeFlag g_finishFlag;

	void PointCloudViewer::Start() const noexcept
	{
		m_worldManager->Start();

		m_threadManager->StartEngineThread(&PointCloudViewer::UpdateTask, this);
	}

	void PointCloudViewer::UpdateTask() const noexcept
	{
		while (!g_finishFlag)
		{
			Time::Update();

			m_worldManager->Update();
		}
	}

	void PointCloudViewer::Stop() const noexcept
	{
		g_finishFlag = true;
		m_threadManager->StopEngineThread();

		m_worldManager->Stop(); // unregister mesh renderers, remove descriptor set, pipelines, pipeline layouts
	}

	PointCloudViewer::~PointCloudViewer()
	{
		// will destroy managers in certain order
		m_inputManager = nullptr;
		m_engineData = nullptr; //delete all internal engine resources
		m_worldManager = nullptr; //delete swapchain, synchronisation, framebuffers
		m_descriptorSetManager = nullptr;
		m_dataManager = nullptr;
		m_memoryManager = nullptr; //free gpu memory
		m_graphicsManager = nullptr; //delete surface, device, instance

		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

#ifdef _DEBUG
		IDXGIDebug1* pDebug = nullptr;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
		{
			pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			pDebug->Release();
		}
#endif

		Logger::Log("Context destroyed\n");
	}

	// always main GUI thread
	void PointCloudViewer::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);

		m_inputManager->HandleWinMessage(hwnd, uMsg, wParam, lParam);
	}
}

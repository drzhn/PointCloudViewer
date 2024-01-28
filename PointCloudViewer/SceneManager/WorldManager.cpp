#include "WorldManager.h"

#include "Components/Camera.h"
#include "RenderManager/PointCloudRenderer.h"
#include "DataManager/DataManager.h"
#include "GameplayComponents/CameraBehaviour.h"
#include "Utils/TimeCounter.h"


namespace PointCloudViewer
{
	WorldManager::WorldManager(HWND gameWindowHandle)
	{
		m_renderManager = std::make_unique<PointCloudRenderer>(gameWindowHandle);
	}

	void WorldManager::Init()
	{
		TIME_PERF("WorldManager init");

		// creating render resources
		m_renderManager->Init();

		m_camera = std::make_unique<GameObject>("camera");
		m_camera->AddComponent(std::make_unique<Camera>(
			*m_camera,
			m_renderManager.get(),
			0.01f,
			1000.f,
			60.f));
		m_camera->AddComponent(std::make_unique<CameraBehaviour>(*m_camera));

	}

	void WorldManager::Start()
	{
		m_renderManager->Start();
	}


	void WorldManager::Update()
	{
		m_renderManager->PreUpdate();

		m_camera->Update();

		m_renderManager->Update();
	}


	void WorldManager::Stop()
	{
		m_renderManager->Stop();
	}

	WorldManager::~WorldManager()
	{
		m_camera = nullptr;
		m_renderManager = nullptr;
	}
}

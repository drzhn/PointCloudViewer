#include "ThreadManager.h"

namespace PointCloudViewer
{
	void ThreadManager::StopEngineThread()
	{
		m_engineThread.join();
	}
}

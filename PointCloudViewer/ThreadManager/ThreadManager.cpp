#include "ThreadManager.h"

namespace PointCloudViewer
{
	void ThreadManager::Stop()
	{
		m_worker.join();
	}
}

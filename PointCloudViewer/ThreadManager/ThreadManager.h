#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H
#include <thread>
#include <vector>

#include "Common/Singleton.h"
#include "Utils/Assert.h"


namespace PointCloudViewer
{
	class ThreadManager : public Singleton<ThreadManager>
	{
	public:
		ThreadManager(): m_hardwareConcurrency(std::thread::hardware_concurrency() - 2)
		{
			m_workers.resize(m_hardwareConcurrency);
		}

		template <typename... Args>
		void StartEngineThread(Args&&... args)
		{
			m_engineThread = std::thread(std::forward<Args>(args)...);
		}

		void StopEngineThread();

		template <typename... Args>
		void StartWorker(int workerIndex, Args&&... args)
		{
			ASSERT(!m_workers[workerIndex].joinable());
			m_workers[workerIndex] = std::thread(std::forward<Args>(args)...);
		}

		void WaitAllWorkers()
		{
			for (uint32_t i = 0; i < m_hardwareConcurrency; i++)
			{
				if (!m_workers[i].joinable())
				{
					continue;
				}
				m_workers[i].join();
			}
		}

		[[nodiscard]] uint32_t GetWorkersCount() const noexcept { return m_hardwareConcurrency; }

	private:
		const uint32_t m_hardwareConcurrency;
		std::vector<std::thread> m_workers;

		std::thread m_engineThread;
	};
}
#endif // THREAD_MANAGER_H

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <array>

#include "Common/CommandQueue.h"
#include "ResourceManager/Texture.h"

#include "LinearMemoryAllocator.h"
#include "Common/Singleton.h"
#include "ResourceManager/Buffers/Buffer.h"


namespace PointCloudViewer
{
	struct BufferUploadPayload
	{
		BufferUploadPayload():
			m_stagingBuffer(std::make_unique<Buffer>(
				m_bufferSize,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				D3D12_HEAP_TYPE_UPLOAD))

		{
		}

		const uint64_t m_bufferSize = 64ull * 1024ull * 1024ull;
		std::unique_ptr<Buffer> m_stagingBuffer;
		uint64_t m_dataSize = 0;
	};

	class MemoryManager : public Singleton<MemoryManager>
	{
	public:
		MemoryManager();

		void PrintStats() const;

		//void LoadDataToBuffer(
		//	std::ifstream& stream,
		//	uint64_t offset,
		//	uint64_t bufferSize,
		//	const Buffer* gpuBuffer) const;

		void LoadDataToBuffer(const std::vector<BufferUploadPayload>& payloads, const Buffer* gpuBuffer) const;

		ComPtr<ID3D12Resource> CreateResource(
			D3D12_HEAP_TYPE heapType,
			const D3D12_RESOURCE_DESC* resourceDesc,
			D3D12_RESOURCE_STATES initialResourceState,
			const D3D12_CLEAR_VALUE* clearValue = nullptr) const;

	private:
		void LoadDataToBufferInternal(uint64_t bufferSize, const Buffer* gpuBuffer, uint64_t bufferOffset) const;

		std::unique_ptr<CommandQueue> m_queue;
		std::array<std::unique_ptr<LinearMemoryAllocator>, 5> m_allocators;
		//std::unique_ptr<Buffer> m_uploadStagingBuffer;
	};
}

#endif

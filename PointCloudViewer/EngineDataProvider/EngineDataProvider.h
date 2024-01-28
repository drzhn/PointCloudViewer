#ifndef ENGINE_DATA_PROVIDER_H
#define ENGINE_DATA_PROVIDER_H

#include "Common/Singleton.h"

#include "CommonEngineStructs.h"
#include "MeshContainer.h"
#include "ResourceManager/ResourceView.h"
#include "ResourceManager/Buffers/ConstantCpuBuffer.h"
#include "ResourceManager/Buffers/DynamicCpuBuffer.h"
#include "ResourceManager/Pipelines/GraphicsPipeline.h"

namespace PointCloudViewer
{
	class EngineDataProvider : public Singleton<EngineDataProvider>
	{
	public:
		EngineDataProvider() = default;
		~EngineDataProvider() = default;

		void Init();

		[[nodiscard]] GraphicsPipeline* GetGizmoAxisDrawerSharedMaterial() const noexcept { return m_gizmoAxisDrawerSharedMaterial.get(); }

		[[nodiscard]] ResourceView* GetEngineDataView(uint32_t index) const noexcept { return m_engineDataBuffer->GetView(index); }

		[[nodiscard]] DynamicCpuBuffer<EngineData>* GetEngineDataBuffer() const noexcept { return m_engineDataBuffer.get(); }

	private:
		//ResourceHandle<SharedMaterial> m_standardSharedMaterial;

		std::unique_ptr<GraphicsPipeline> m_gizmoAxisDrawerSharedMaterial;

		std::unique_ptr<DynamicCpuBuffer<EngineData>> m_engineDataBuffer;
	};
}

#endif // ENGINE_DATA_PROVIDER_H

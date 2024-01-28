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

		//[[nodiscard]] ResourceHandle<SharedMaterial> GetStandardSharedMaterial() const noexcept { return m_standardSharedMaterial; }
		//[[nodiscard]] ResourceHandle<ComputePipeline> GetMipsGenerationComputePipeline() const noexcept { return m_generateMipsComputePipeline; }
		[[nodiscard]] GraphicsPipeline* GetGizmoAxisDrawerSharedMaterial() const noexcept { return m_gizmoAxisDrawerSharedMaterial.get(); }
		[[nodiscard]] GraphicsPipeline* GetDDGIDeferredShadingSharedMaterial() const noexcept { return m_ddgiDeferredShadingSharedMaterial.get(); }
		[[nodiscard]] GraphicsPipeline* GetDeferredShadingSharedMaterial() const noexcept { return m_deferredShadingSharedMaterial.get(); }

		[[nodiscard]] ResourceView* GetNullTextureView() const noexcept { return m_nullTextureView.get(); }
		[[nodiscard]] ResourceView* GetMaterialsDataView() const noexcept { return m_materials->GetView(); }
		[[nodiscard]] ResourceView* GetEngineDataView(uint32_t index) const noexcept { return m_engineDataBuffer->GetView(index); }

		[[nodiscard]] MeshContainer* GetMeshContainer() const { return m_meshContainer.get(); }

		[[nodiscard]] DynamicCpuBuffer<EngineData>* GetEngineDataBuffer() const noexcept { return m_engineDataBuffer.get(); }

	private:
		//ResourceHandle<SharedMaterial> m_standardSharedMaterial;

		std::unique_ptr<GraphicsPipeline> m_gizmoAxisDrawerSharedMaterial;

		std::unique_ptr<GraphicsPipeline> m_ddgiDeferredShadingSharedMaterial;

		std::unique_ptr<GraphicsPipeline> m_deferredShadingSharedMaterial;

		std::unique_ptr<ResourceView> m_nullTextureView;

		std::unique_ptr<ConstantCpuBuffer<StandardMaterialData>> m_materials;

		std::unique_ptr<DynamicCpuBuffer<EngineData>> m_engineDataBuffer;

		std::unique_ptr<MeshContainer> m_meshContainer;
	};
}

#endif // ENGINE_DATA_PROVIDER_H

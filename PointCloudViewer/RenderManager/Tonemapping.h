#ifndef TONEMAPPING_H
#define TONEMAPPING_H

#include <dxgiformat.h>
#include <memory>

#include "CommonEngineStructs.h"
#include "ResourceManager/Buffers/DynamicCpuBuffer.h"
#include "ResourceManager/Buffers/UAVGpuBuffer.h"
#include "ResourceManager/Pipelines/GraphicsPipeline.h"
#include "ResourceManager/Pipelines/ComputePipeline.h"


namespace PointCloudViewer
{
	class IRenderer;
	class RenderTexture;
	class ComputePipeline;
	class SharedMaterial;
	class UAVTexture;
	class Buffer;
	class ResourceView;


	class Tonemapping
	{
	public:
		Tonemapping() = delete;
		explicit Tonemapping(
			IRenderer* renderManager,
			RenderTexture* hdrRenderTarget,
			DXGI_FORMAT hdrRTVFormat,
			DXGI_FORMAT ldrRTVFormat,
			DXGI_FORMAT depthFormat
		);
		~Tonemapping() = default;

		void Render(ID3D12GraphicsCommandList* commandList, uint32_t frameIndex, const RenderTexture* currentBackBuffer) const;
		HDRDownScaleConstants* GetConstantsPtr() noexcept { return &m_constantsValues; }
		void UpdateConstants(uint32_t frameIndex) const;
	private:
		std::unique_ptr<ComputePipeline> m_hdrDownscaleFirstPassComputePipeline;
		std::unique_ptr<ComputePipeline> m_hdrDownscaleSecondPassComputePipeline;
		std::unique_ptr<GraphicsPipeline> m_hdrToLdrTransitionGraphicsPipeline;

		std::unique_ptr<UAVTexture> m_hrdDownScaledTexture;
		std::unique_ptr<UAVGpuBuffer> m_hdrLuminationBuffer;
		std::unique_ptr<UAVGpuBuffer> m_hdrPrevLuminationBuffer;

		HDRDownScaleConstants m_constantsValues;
		std::unique_ptr<DynamicCpuBuffer<HDRDownScaleConstants>> m_constantsBuffer;

		DXGI_FORMAT m_hdrRTVFormat;
		DXGI_FORMAT m_ldrRTVFormat;
		DXGI_FORMAT m_depthFormat;

		uint32_t m_screenWidth;
		uint32_t m_screenHeight;

		RenderTexture* m_hdrRenderTarget;
		uint32_t m_groupSize;
	};
}

#endif // TONEMAPPING_H

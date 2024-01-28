#include "EngineDataProvider.h"

#include <d3d12.h>

#include "d3dx12.h"
#include "DescriptorManager/DescriptorManager.h"
#include "GraphicsManager/GraphicsManager.h"
#include "ResourceManager/Texture.h"
#include "Utils/TimeCounter.h"
#include "SceneManager/WorldManager.h"

namespace PointCloudViewer
{
	void EngineDataProvider::Init()
	{
		TIME_PERF("EngineDataProvider init")

		EngineSamplersProvider::InitSamplers();
		DXGI_FORMAT mainRTVFormat = IRenderer::GetHDRRenderTextureFormat();
		DXGI_FORMAT swapchainFormat = IRenderer::GetSwapchainFormat();
		DXGI_FORMAT mainGBufferFormat = IRenderer::GetGBufferFormat();
		DXGI_FORMAT mainDSVFormat = IRenderer::GetDepthFormat();


		// Gizmo Axis draw 
		{
			GraphicsPipelineArgs args = {
				"shaders/gizmoAxisDrawer.hlsl",
				JoyShaderTypeVertex | JoyShaderTypePixel,
				false,
				false,
				false,
				D3D12_CULL_MODE_NONE,
				D3D12_COMPARISON_FUNC_NEVER,
				CD3DX12_BLEND_DESC(D3D12_DEFAULT),
				{
					swapchainFormat
				},
				1,
				mainDSVFormat,
				D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			};
			m_gizmoAxisDrawerSharedMaterial = std::make_unique<GraphicsPipeline>(args);
		}

		{
			m_engineDataBuffer = std::make_unique<DynamicCpuBuffer<EngineData>>(WorldManager::Get()->GetRenderer().GetFrameCount());
		}
	}
}

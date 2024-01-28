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

		// Null texture view 
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D = {
					.MostDetailedMip = 0,
					.MipLevels = 1,
					.PlaneSlice = 0,
					.ResourceMinLODClamp = 0
				}
			};
			m_nullTextureView = std::make_unique<ResourceView>(desc, nullptr);
		}

		// Materials data that we will pass to GPU
		{
			m_materials = std::make_unique<ConstantCpuBuffer<StandardMaterialData>>();
		}

		{
			m_engineDataBuffer = std::make_unique<DynamicCpuBuffer<EngineData>>(WorldManager::Get()->GetRenderer().GetFrameCount());
		}

		{
			m_meshContainer = std::make_unique<MeshContainer>();
		}

		// DDGI Deferred shading processing
		{
			GraphicsPipelineArgs args = {
				"shaders/ddgi/ddgi_deferred_shading.hlsl",
				JoyShaderTypeVertex | JoyShaderTypePixel,
				false,
				false,
				false,
				D3D12_CULL_MODE_NONE,
				D3D12_COMPARISON_FUNC_GREATER_EQUAL,
				CD3DX12_BLEND_DESC(D3D12_DEFAULT),
				{
					mainGBufferFormat,
				},
				1,
				mainDSVFormat,
				D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			};
			m_ddgiDeferredShadingSharedMaterial = std::make_unique<GraphicsPipeline>(args);
		}

		// Deferred shading processing
		{
			GraphicsPipelineArgs args = {

				"shaders/basic_renderer/deferred_shading.hlsl",
				JoyShaderTypeVertex | JoyShaderTypePixel,
				false,
				false,
				false,
				D3D12_CULL_MODE_NONE,
				D3D12_COMPARISON_FUNC_GREATER_EQUAL,
				CD3DX12_BLEND_DESC(D3D12_DEFAULT),
				{
					mainGBufferFormat,
				},
				1,
				mainDSVFormat,
				D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			};
			m_deferredShadingSharedMaterial = std::make_unique<GraphicsPipeline>(args);
		}
	}
}

#include "PointCloudHandler.h"

#include "CommonEngineStructs.h"
#include "IRenderer.h"
#include "MemoryManager/MemoryManager.h"

PointCloudViewer::PointCloudHandler::PointCloudHandler()
{
	m_pointCloudBuffer = std::make_unique<UAVGpuBuffer>(
		8,
		sizeof(Vertex)
	);
	Vertex data[8] = {
		{{1, 1, 1, 1}, {1, 1, 1, 1}},
		{{1, 1, -1, 1}, {1, 1, 1, 1}},
		{{1, -1, 1, 1}, {1, 1, 1, 1}},
		{{1, -1, -1, 1}, {1, 1, 1, 1}},
		{{-1, 1, 1, 1}, {1, 1, 1, 1}},
		{{-1, 1, -1, 1}, {1, 1, 1, 1}},
		{{-1, -1, 1, 1}, {1, 1, 1, 1}},
		{{-1, -1, -1, 1}, {1, 1, 1, 1}},
	};
	MemoryManager::Get()->LoadDataToBuffer(
		data,
		sizeof(data),
		m_pointCloudBuffer->GetBuffer(),
		0
	);

	GraphicsPipelineArgs args = {
		.shaderPath = "shaders/point_cloud.hlsl",
		.shaderTypes = JoyShaderTypePixel | JoyShaderTypeVertex,
		.hasVertexInput = false,
		.depthTest = false,
		.depthWrite = false,
		.cullMode = D3D12_CULL_MODE_NONE,
		.depthComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS,
		.blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		.renderTargetsFormats =
		{
			IRenderer::GetHDRRenderTextureFormat()
		},
		.renderTargetsFormatsSize = 1,
		.topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
	};
	m_graphicsPipeline = std::make_unique<GraphicsPipeline>(args);
}

#include "PointCloudHandler.h"

#include "CommonEngineStructs.h"
#include "IRenderer.h"
#include "MemoryManager/MemoryManager.h"
#include "ThreadManager/ThreadManager.h"
#include "Utils/TimeCounter.h"

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

	TIME_PERF_HIGHRES("File read");


	FILE* fp;
	fopen_s(&fp, "Data/test/stgallencathedral_station1_intensity_rgb.txt", "r");
	fseek(fp, 0L, SEEK_END);
	uint64_t fileSize = ftell(fp);
	fclose(fp);

	const uint32_t concurrency = ThreadManager::Get()->GetWorkersCount();
	uint64_t fileChunk = fileSize / concurrency;

	for (uint32_t workerIndex = 0; workerIndex < concurrency; workerIndex++)
	{
		ThreadManager::Get()->StartWorker(workerIndex, [workerIndex, fileChunk, concurrency]()
		{
			FILE* fp;
			fopen_s(&fp, "Data/test/stgallencathedral_station1_intensity_rgb.txt", "r");
			uint64_t startPosition = workerIndex * fileChunk;
			if (workerIndex > 0)
			{
				fseek(fp, workerIndex * fileChunk, 0);
				while (fgetc(fp) != '\n')
				{
				}
				startPosition = ftell(fp);
			}
			uint64_t endPosition = (workerIndex + 1) * fileChunk;
			if (workerIndex == concurrency)
			{
				endPosition = ~0;
			}

			fseek(fp, startPosition, 0);

			float x;
			float y;
			float z;
			int ii;
			int r;
			int g;
			int b;
			int res;
			int linesRead = 0;
			while (
				fscanf_s(fp, "%f %f %f %d %d %d %d", &x, &y, &z, &ii, &r, &g, &b) == 7 &&
				ftell(fp) < endPosition
			)
			{
				linesRead++;
			}
			Logger::LogFormat("Lines read: %d\n", linesRead);
			fclose(fp);
		});
	}

	ThreadManager::Get()->WaitAllWorkers();
}

#include "PointCloudHandler.h"

#include "CommonEngineStructs.h"
#include "IRenderer.h"
#include "MemoryManager/MemoryManager.h"
#include "ThreadManager/ThreadManager.h"
#include "Utils/TimeCounter.h"

int char_to_int(char s)
{
	return static_cast<int>(s) - 48;
}

bool is_number(char s)
{
	return static_cast<int>(s) >= 48 && static_cast<int>(s) <= 57;
}

int read_floats(FILE* fp, float* out)
{
	int floats_read = 0;
	int sign = 1;
	int number = 0;
	bool is_fraction = false;
	int divider = 1;
	char c = static_cast<char>(fgetc(fp));

	while (c != '\n' && c != EOF)
	{
		if (c == '-')
		{
			sign = -1;
		}
		else if (is_number(c))
		{
			number = number * 10 + char_to_int(c);
			if (is_fraction)
			{
				divider *= 10;
			}
		}
		else if (c == '.')
		{
			is_fraction = true;
		}
		else
		{
			out[floats_read] = static_cast<float>(number * sign) / divider;
			floats_read += 1;
			sign = 1;
			number = 0;
			is_fraction = false;
			divider = 1;
		}

		c = static_cast<char>(fgetc(fp));
	}
	out[floats_read] = static_cast<float>(number * sign) / divider;
	floats_read++;

	return floats_read;
}

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

	const uint32_t concurrency = ThreadManager::Get()->GetWorkersCount();

	for (uint32_t workerIndex = 0; workerIndex < concurrency; workerIndex++)
	{
		ThreadManager::Get()->StartWorker(workerIndex, [workerIndex, concurrency]()
		{
			FILE* fp;
			fopen_s(&fp, "Data/test/stgallencathedral_station1_intensity_rgb.txt", "r");
			fseek(fp, 0L, SEEK_END);
			uint64_t fileSize = ftell(fp);
			uint64_t fileChunk = fileSize / concurrency;

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
				endPosition = fileSize;
			}

			fseek(fp, startPosition, 0);

			float numbers[8];
			int linesRead = 0;
			while (read_floats(fp, numbers) == 7 &&
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

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

int read_floats(const char* data, float* out, int& currentPosition, int endPosition)
{
	int floats_read = 0;
	int sign = 1;
	int number = 0;
	bool is_fraction = false;
	int divider = 1;
	bool isReadingNumber = false;

	char c = data[currentPosition];
	currentPosition++;

	while (c != '\n' && currentPosition < endPosition)
	{
		if (c == '-')
		{
			sign = -1;
		}
		else if (is_number(c))
		{
			isReadingNumber = true;
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
			if (isReadingNumber)
			{
				out[floats_read] = static_cast<float>(number * sign) / divider;
				floats_read += 1;
			}

			sign = 1;
			number = 0;
			is_fraction = false;
			divider = 1;
			isReadingNumber = false;
		}

		c = data[currentPosition];
		currentPosition++;
	}

	if (isReadingNumber)
	{
		out[floats_read] = static_cast<float>(number * sign) / divider;
		floats_read += 1;
	}

	return floats_read;
}

PointCloudViewer::PointCloudHandler::PointCloudHandler()
{
	m_pointCloudBuffer = std::make_unique<UAVGpuBuffer>(
		8,
		sizeof(Vertex)
	);
	Vertex vertices[8] = {
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
		vertices,
		sizeof(vertices),
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

	FILE* fp;
	fopen_s(&fp, "Data/test/stgallencathedral_station1_intensity_rgb.txt", "r");
	//fopen_s(&fp, "Data/test/test.txt", "rb");
	fseek(fp, 0L, SEEK_END);
	uint64_t fileSize = ftell(fp);
	char* data = static_cast<char*>(malloc(fileSize + 1));
	fseek(fp, 0L, 0);
	fread(data, fileSize, 1, fp);
	fclose(fp);

	data[fileSize] = '\n';
	fileSize++;

	std::atomic_int totalLinesRead = 0;

	for (uint32_t workerIndex = 0; workerIndex < concurrency; workerIndex++)
	{
		ThreadManager::Get()->StartWorker(workerIndex, [data, workerIndex, concurrency, fileSize, &totalLinesRead]()
		{
			const uint64_t fileChunk = fileSize / concurrency;
			uint64_t startPosition = workerIndex * fileChunk;
			if (workerIndex > 0)
			{
				char c;
				do
				{
					c = data[startPosition];
					startPosition++;
				}
				while (c != '\n' && startPosition != fileSize);
			}
			uint64_t endPosition = (workerIndex + 1) * fileChunk;
			if (workerIndex + 1 == concurrency)
			{
				endPosition = fileSize;
			}
			else
			{
				char c;
				do
				{
					c = data[endPosition];
					endPosition++;
				}
				while (c != '\n' && endPosition != fileSize);
			}

			float numbers[8];
			int linesRead = 0;
			int currentPosition = startPosition;
			while (read_floats(data,
			                   numbers,
			                   currentPosition,
			                   endPosition) == 7
			)
			{
				linesRead++;
			}
			Logger::LogFormat("Lines read: %d\n", linesRead);
			totalLinesRead.fetch_add(linesRead);
		});
	}

	ThreadManager::Get()->WaitAllWorkers();
	Logger::LogFormat("Total ines read: %d\n", totalLinesRead.load());
	free(data);
}

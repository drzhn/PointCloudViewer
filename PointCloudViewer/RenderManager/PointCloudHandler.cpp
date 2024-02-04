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

int read_floats(const char* data, float* out, uint64_t& currentPosition, uint64_t endPosition)
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

	const uint32_t concurrency = ThreadManager::Get()->GetWorkersCount();
	std::vector<BufferUploadPayload> bufferUploadPayloads(concurrency);
	std::atomic_uint64_t totalLinesRead = 0;

	{
		TIME_PERF_HIGHRES("File read");

		FILE* fp;
		fopen_s(&fp, "Data/test/stgallencathedral_station1_intensity_rgb.txt", "rb");
		//fopen_s(&fp, "Data/test/test.txt", "rb");
		fseek(fp, 0L, SEEK_END);
		uint64_t fileSize = ftell(fp);
		char* fileData = static_cast<char*>(malloc(fileSize));
		fseek(fp, 0L, 0);
		fread(fileData, fileSize, 1, fp);
		fclose(fp);


		for (uint32_t workerIndex = 0; workerIndex < concurrency; workerIndex++)
		{
			ThreadManager::Get()->StartWorker(workerIndex, [fileData, workerIndex, concurrency, fileSize, &totalLinesRead, &bufferUploadPayloads]()
			{
				const uint64_t fileChunk = fileSize / concurrency;
				uint64_t startPosition = workerIndex * fileChunk;
				if (workerIndex > 0)
				{
					char c;
					do
					{
						c = fileData[startPosition];
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
						c = fileData[endPosition];
						endPosition++;
					}
					while (c != '\n' && endPosition != fileSize);
				}

				const MappedAreaHandle mappedHandle = bufferUploadPayloads[workerIndex].m_stagingBuffer->Map();
				Vertex* bufferData = static_cast<Vertex*>(mappedHandle.GetPtr());

				float numbers[8];
				int linesRead = 0;
				uint64_t currentPosition = startPosition;
				while (read_floats(fileData, numbers, currentPosition, endPosition) == 7)
				{
					bufferData[linesRead].position = {numbers[0], numbers[1], numbers[2]};
					linesRead++;
				}
				bufferUploadPayloads[workerIndex].m_dataSize = linesRead * sizeof(Vertex);

				Logger::LogFormat("Lines read: %d\n", linesRead);
				totalLinesRead.fetch_add(linesRead);
			});
		}

		ThreadManager::Get()->WaitAllWorkers();
		Logger::LogFormat("Total ines read: %d\n", totalLinesRead.load());
		free(fileData);
	}

	{
		TIME_PERF_HIGHRES("Uploading data");

		m_pointCloudBuffer = std::make_unique<UAVGpuBuffer>(
		   totalLinesRead.load(),
		   sizeof(Vertex)
	   );
		m_pointsNumber = totalLinesRead.load();

		MemoryManager::Get()->LoadDataToBuffer(bufferUploadPayloads, m_pointCloudBuffer->GetBuffer());
	}
}

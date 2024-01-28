#include "DataManager.h"

#include <fstream>
#include <vector>

#include "Common/HashDefs.h"
#include "Utils/FileUtils.h"
#include "Utils/TimeCounter.h"

namespace PointCloudViewer
{
	DataManager::DataManager() : m_dataPath(std::filesystem::absolute(R"(Data/)"))
	{
		TIME_PERF("DataManager ctor")
	}

	std::vector<char> DataManager::GetData(const std::string& path, bool shouldReadRawData, uint32_t offset) const
	{
		if (shouldReadRawData)
		{
			return ReadFile((m_dataPath / (path + ".data")).generic_string(), offset);
		}
		else
		{
			return ReadFile((m_dataPath / path).generic_string(), offset);
		}
	}

	bool DataManager::HasRawData(const std::string& path) const
	{
		return std::filesystem::exists(m_dataPath / (path + ".data"));
	}

	void DataManager::GetWFilename(const std::string& path, std::wstring& filename)
	{
		filename = std::filesystem::path(path).stem().generic_wstring();
	}

	std::ifstream DataManager::GetFileStream(const std::string& path, bool shouldReadRawData) const
	{
		const auto delimiterPos = path.find_first_of(':');
		std::string dataPath = path;
		if (delimiterPos != std::string::npos)
		{
			dataPath = dataPath.substr(0, delimiterPos);
		}
		dataPath += shouldReadRawData ? ".data" : "";
		std::ifstream stream = GetStream((m_dataPath / dataPath).generic_string());
		return stream;
	}
}

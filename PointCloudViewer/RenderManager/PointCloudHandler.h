#ifndef POINTCLOUD_HANDLER_H
#define POINTCLOUD_HANDLER_H

#include <memory>

#include "ResourceManager/Buffers/UAVGpuBuffer.h"
#include "ResourceManager/Pipelines/GraphicsPipeline.h"

namespace PointCloudViewer
{
	class PointCloudHandler
	{
	public:
		PointCloudHandler();
		[[nodiscard]] GraphicsPipeline* GetGraphicsPipeline() const noexcept { return m_graphicsPipeline.get(); }
		[[nodiscard]] UAVGpuBuffer* GetPointBuffer() const noexcept { return m_pointCloudBuffer.get(); }

	private:
		std::unique_ptr<UAVGpuBuffer> m_pointCloudBuffer;
		std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;
	};
}

#endif // POINTCLOUD_HANDLER_H

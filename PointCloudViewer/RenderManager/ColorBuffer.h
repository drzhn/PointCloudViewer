#ifndef G_BUFFER_H
#define G_BUFFER_H
#include <memory>

#include "ResourceManager/Texture.h"

struct ID3D12GraphicsCommandList;

namespace PointCloudViewer
{
	class ResourceView;

	class ColorBuffer
	{
	public:
		ColorBuffer() = delete;
		explicit ColorBuffer(uint32_t width, uint32_t height);

		void BarrierColorToRead(ID3D12GraphicsCommandList* commandList) const;
		void BarrierColorToWrite(ID3D12GraphicsCommandList* commandList) const;
		void BarrierDepthToRead(ID3D12GraphicsCommandList* commandList) const;
		void BarrierDepthToWrite(ID3D12GraphicsCommandList* commandList) const;

		[[nodiscard]] ResourceView* GetColorRTV() const noexcept { return m_colorTexture->GetRTV(); }
		[[nodiscard]] ResourceView* GetDepthDSV() const noexcept { return m_depthTexture->GetDSV(); }

		[[nodiscard]] RenderTexture* GetColorTexture() const noexcept { return m_colorTexture.get(); }
		[[nodiscard]] DepthTexture* GetDepthTexture() const noexcept { return m_depthTexture.get(); }

	private:
		uint32_t m_width = 0;
		uint32_t m_height = 0;

		std::unique_ptr<RenderTexture> m_colorTexture;
		std::unique_ptr<DepthTexture> m_depthTexture;
	};
}

#endif // G_BUFFER_H

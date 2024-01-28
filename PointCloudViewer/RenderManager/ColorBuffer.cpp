#include "ColorBuffer.h"

#include "IRenderer.h"
#include "Utils/GraphicsUtils.h"

namespace PointCloudViewer
{
	ColorBuffer::ColorBuffer(uint32_t width, uint32_t height) :
		m_width(width),
		m_height(height)
	{
		m_colorTexture = std::make_unique<RenderTexture>(
			m_width,
			m_height,
			IRenderer::GetHDRRenderTextureFormat(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_DEFAULT
		);

		m_depthTexture = std::make_unique<DepthTexture>(
			m_width,
			m_height,
			IRenderer::GetDepthFormat(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_HEAP_TYPE_DEFAULT
		);
	}

	void ColorBuffer::BarrierColorToRead(ID3D12GraphicsCommandList* commandList) const
	{
		GraphicsUtils::Barrier(
			commandList, m_colorTexture->GetImageResource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void ColorBuffer::BarrierColorToWrite(ID3D12GraphicsCommandList* commandList) const
	{
		GraphicsUtils::Barrier(
			commandList, m_colorTexture->GetImageResource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void ColorBuffer::BarrierDepthToRead(ID3D12GraphicsCommandList* commandList) const
	{
		GraphicsUtils::Barrier(
			commandList, m_depthTexture->GetImageResource().Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void ColorBuffer::BarrierDepthToWrite(ID3D12GraphicsCommandList* commandList) const
	{
		GraphicsUtils::Barrier(
			commandList, m_depthTexture->GetImageResource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}
}

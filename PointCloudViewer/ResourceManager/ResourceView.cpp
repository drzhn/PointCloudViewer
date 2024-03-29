#include "ResourceView.h"


#include "GraphicsManager/GraphicsManager.h"
#include "DescriptorManager/DescriptorManager.h"

namespace PointCloudViewer
{
	ResourceView::ResourceView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource* resource) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateDepthStencilView(
			resource,
			&desc,
			m_cpuHandle);
	}

	ResourceView::ResourceView(const D3D12_SAMPLER_DESC& desc) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateSampler(
			&desc,
			m_cpuHandle);
	}

	ResourceView::ResourceView(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateConstantBufferView(
			&desc,
			m_cpuHandle);
	}

	ResourceView::ResourceView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D12Resource* resource) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateUnorderedAccessView(
			resource,
			nullptr,
			&desc,
			m_cpuHandle);
	}

	ResourceView::ResourceView(const D3D12_RENDER_TARGET_VIEW_DESC& desc, ID3D12Resource* resource) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateRenderTargetView(
			resource,
			&desc,
			m_cpuHandle);
	}

	ResourceView::ResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, ID3D12Resource* resource) :
		m_type(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		DescriptorManager::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descriptorIndex, m_cpuHandle, m_gpuHandle);

		GraphicsManager::Get()->GetDevice()->CreateShaderResourceView(
			resource,
			&desc,
			m_cpuHandle);
	}

}

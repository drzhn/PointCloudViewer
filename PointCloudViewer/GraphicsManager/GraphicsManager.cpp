#include "GraphicsManager/GraphicsManager.h"

#include "Utils/Assert.h"
#include "Utils/TimeCounter.h"

namespace PointCloudViewer
{
	GraphicsManager::GraphicsManager() 
	{
		TIME_PERF("GraphicsManager ctor")
		UINT createFactoryFlags = 0;
#if defined(GRAPHICS_DEBUG)

		ASSERT_SUCC(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
		m_debugController->EnableDebugLayer();
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

#endif

		ASSERT_SUCC(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
		ASSERT_SUCC(m_dxgiFactory->CheckFeatureSupport(
			DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&m_allowTearing,
			sizeof(uint32_t)));

		ComPtr<IDXGIAdapter1> dxgiAdapter1;
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; m_dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
					D3D_FEATURE_LEVEL_12_1,
					__uuidof(ID3D12Device),
					nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				dxgiAdapter1.As(&m_physicalDevice);
			}
		}
		ASSERT(m_physicalDevice != nullptr);


		DXGI_QUERY_VIDEO_MEMORY_INFO local_info;
		m_physicalDevice->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local_info);
		DXGI_QUERY_VIDEO_MEMORY_INFO non_local_info;
		m_physicalDevice->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &non_local_info);

		ASSERT_SUCC(D3D12CreateDevice(m_physicalDevice.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_logicalDevice)));

		ASSERT_SUCC(m_logicalDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_featureSupport, sizeof(m_featureSupport)));

		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {D3D_SHADER_MODEL_6_7};
		ASSERT_SUCC(m_logicalDevice->CheckFeatureSupport(
			D3D12_FEATURE_SHADER_MODEL,
			&shaderModel,
			sizeof(D3D12_FEATURE_DATA_SHADER_MODEL)));

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(GraphicsManager::Get()->GetDevice()->CheckFeatureSupport(
			D3D12_FEATURE_ROOT_SIGNATURE,
			&featureData,
			sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}
		m_highestRootSignatureVersion = featureData.HighestVersion;

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
		ASSERT_SUCC(m_logicalDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5)));
		ASSERT(options5.RaytracingTier > D3D12_RAYTRACING_TIER_1_0);

		// Enable debug messages in debug mode.
#if defined(GRAPHICS_DEBUG)
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_logicalDevice.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				// I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				// This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				// This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ASSERT_SUCC(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif

		//D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		//msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//msQualityLevels.SampleCount = 4;
		//msQualityLevels.Flags =
		//	D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		//msQualityLevels.NumQualityLevels = 0;
		//ASSERT_SUCC(m_logicalDevice->CheckFeatureSupport(
		//	D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		//	&msQualityLevels,
		//	sizeof(msQualityLevels)));
		//m_m4xMsaaQuality = msQualityLevels.NumQualityLevels;
		//ASSERT(m_m4xMsaaQuality > 0);
	}
}

#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <array>
#include <set>
#include <memory>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "ColorBuffer.h"
#include "CommonEngineStructs.h"
#include "PointCloudHandler.h"
#include "RenderManager/IRenderer.h"
#include "Common/CommandQueue.h"
#include "RenderManager/Tonemapping.h"


using Microsoft::WRL::ComPtr;

namespace PointCloudViewer
{
	enum class EngineBindingType;

	class CommandQueue;
	class Mesh;
	class Camera;
	class Texture;
	class RenderTexture;
	class ResourceView;
	class DepthTexture;

	class PointCloudRenderer final : public IRenderer
	{
	public:
		PointCloudRenderer() = delete;

		explicit PointCloudRenderer(HWND windowHandle);

		~PointCloudRenderer() override = default;

		void Init() override;

		void Start() const override;

		void Stop() override;

		void PreUpdate() override;

		void Update() override;

		void DrawGui(ID3D12GraphicsCommandList* commandList, const ViewProjectionMatrixData* viewProjectionData) const;

		void RegisterCamera(Camera* camera) override;

		void UnregisterCamera(Camera* camera) override;

		[[nodiscard]] float GetAspect() const noexcept override;

		[[nodiscard]] float GetWidth_f() const noexcept override;
		[[nodiscard]] float GetHeight_f() const noexcept override;

		[[nodiscard]] uint32_t GetWidth() const noexcept override;
		[[nodiscard]] uint32_t GetHeight() const noexcept override;

		[[nodiscard]] const uint32_t GetFrameCount() const noexcept override { return FRAME_COUNT; }
		[[nodiscard]] const uint32_t GetCurrentFrameIndex() const noexcept override { return m_currentFrameIndex; }

	private:
		static void CopyRTVResource(
			ID3D12GraphicsCommandList* commandList,
			ID3D12Resource* rtvResource,
			ID3D12Resource* copyResource
		);

	private:
		static constexpr uint32_t FRAME_COUNT = 3;

		ComPtr<IDXGISwapChain3> m_swapChain;

		std::array<std::unique_ptr<RenderTexture>, FRAME_COUNT> m_swapchainRenderTargets;

		std::unique_ptr<ColorBuffer> m_colorBuffer;
		std::unique_ptr<Tonemapping> m_tonemapping;

		std::unique_ptr<PointCloudHandler> m_pointCloudHandler;

		Camera* m_currentCamera;

		std::unique_ptr<CommandQueue> m_queue;
		uint32_t m_currentFrameIndex;

		uint32_t m_width;
		uint32_t m_height;

		uint32_t m_imguiDescriptorIndex;
	};
}

#endif //RENDER_MANAGER_H

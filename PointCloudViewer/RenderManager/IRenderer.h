#ifndef IRENDERMANAGER_H
#define IRENDERMANAGER_H

#include <dxgi1_6.h>

namespace PointCloudViewer
{
	class Camera;

	class IRenderer 
	{
	public:
		IRenderer(HWND windowHandle):
			m_windowHandle(windowHandle)
		{
		}

		virtual ~IRenderer() = default;

		virtual void Init() = 0;
		virtual void Start() const = 0;
		virtual void Stop() = 0;
		virtual void PreUpdate() = 0;
		virtual void Update() = 0;

		virtual void RegisterCamera(Camera* camera) = 0;
		virtual void UnregisterCamera(Camera* camera) = 0;

		[[nodiscard]] virtual float GetAspect() const noexcept = 0;
		[[nodiscard]] virtual float GetWidth_f() const noexcept = 0;
		[[nodiscard]] virtual float GetHeight_f() const noexcept = 0;
		[[nodiscard]] virtual uint32_t GetWidth() const noexcept = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const noexcept = 0;
		[[nodiscard]] virtual const uint32_t GetFrameCount() const noexcept = 0;
		[[nodiscard]] virtual const uint32_t GetCurrentFrameIndex() const noexcept = 0;


		[[nodiscard]] static DXGI_FORMAT GetHDRRenderTextureFormat() noexcept { return hdrRenderTextureFormat; }
		[[nodiscard]] static DXGI_FORMAT GetSwapchainFormat() noexcept { return swapchainFormat; }
		[[nodiscard]] static DXGI_FORMAT GetGBufferFormat() noexcept { return gBufferFormat; }
		[[nodiscard]] static DXGI_FORMAT GetDepthFormat() noexcept { return depthFormat; }
		[[nodiscard]] static DXGI_FORMAT GetDepthUAVFormat() noexcept { return depthUavFormat; }

	protected:
		static constexpr DXGI_FORMAT hdrRenderTextureFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		static constexpr DXGI_FORMAT swapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr DXGI_FORMAT gBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		static constexpr DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;
		static constexpr DXGI_FORMAT depthUavFormat = DXGI_FORMAT_R32_FLOAT;

		HWND m_windowHandle{};
	};
}
#endif // IRENDERMANAGER_H

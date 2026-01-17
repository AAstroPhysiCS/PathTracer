#pragma once

#include "Core/Base.h"
#include "RenderContext.h"
#include "RenderDevice.h"
#include "RendererAPI.h"

namespace PathTracer {

	struct ComputeDrawCommand;

	struct RendererCreateInfo {
		uint32_t Width;
		uint32_t Height;
		RendererAPI TargetAPI;

		std::shared_ptr<Window> Window;
	};

	class Renderer {
	public:
		Renderer(const RendererCreateInfo& createInfo)
			: m_CreateInfo(createInfo) {
		}
		virtual ~Renderer() = default;

		virtual void Initialize();

		virtual void DispatchComputeDrawCommand(const ComputeDrawCommand& cmd) = 0;
		
		virtual void BeginFrame() = 0;
		virtual void RenderToSwapChain(const std::shared_ptr<Texture>& input) = 0;
		virtual void EndFrame() = 0;

		virtual void Resize(uint32_t newWidth, uint32_t newHeight) = 0;
		virtual void Shutdown() = 0;

		inline RendererAPI GetTargetAPI() const { return m_CreateInfo.TargetAPI; }
		inline std::shared_ptr<Window> GetWindow() const { return m_CreateInfo.Window; }

		inline const std::unique_ptr<RenderContext>& GetRenderContext() const { return m_Context; }
		inline const std::shared_ptr<RenderDevice>& GetRenderDevice() const { return m_RenderDevice; }

		inline uint32_t GetWidth() const { return m_CreateInfo.Width; }
		inline uint32_t GetHeight() const { return m_CreateInfo.Height; }
	private:
		std::unique_ptr<RenderContext> m_Context;
		std::shared_ptr<RenderDevice> m_RenderDevice;

		RendererCreateInfo m_CreateInfo;
	};
}
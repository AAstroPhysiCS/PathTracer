#pragma once

#include "Core/Base.h"

namespace PathTracer {

	class Window;

	struct RenderContextCreateInfo {
		std::shared_ptr<Window> Window;
		bool VSyncEnabled = true;
	};

	class RenderContext {
	public:
		RenderContext(const RenderContextCreateInfo& createInfo);
		virtual ~RenderContext() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		const std::shared_ptr<Window>& GetWindow() const { return m_CreateInfo.Window; }
		bool IsVsyncEnabled() const { return m_CreateInfo.VSyncEnabled; }
	private:
		RenderContextCreateInfo m_CreateInfo;
	};
}
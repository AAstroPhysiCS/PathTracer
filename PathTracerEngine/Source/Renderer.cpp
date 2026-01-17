#include "Renderer.h"

#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLRenderDevice.h"

namespace PathTracer {
	
	void Renderer::Initialize() {
		switch (m_CreateInfo.TargetAPI) {
			case RendererAPI::OpenGL:
				m_Context = std::make_unique<OpenGLContext>(RenderContextCreateInfo{
					.Window = m_CreateInfo.Window
				});
				m_RenderDevice = std::make_shared<OpenGLRenderDevice>(RenderDeviceCreateInfo{});
				break;
			default:
				PT_ASSERT(false, "Renderer API::None is currently not supported!");
				break;
		}

		m_Context->Initialize();
		m_RenderDevice->Initialize();
	}
}
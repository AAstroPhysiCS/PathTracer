#pragma once

#include "glad/glad.h"

#include "RenderContext.h"

namespace PathTracer {

	class OpenGLContext final : public RenderContext {
	public:
		OpenGLContext(const RenderContextCreateInfo& createInfo);
		virtual ~OpenGLContext() = default;

		void Initialize() override;
		void Shutdown() override;
	private:
		static void GLAPIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
			GLsizei length, const GLchar* message, const void* userParam);
	};
}
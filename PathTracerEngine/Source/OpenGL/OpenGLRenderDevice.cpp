#include "OpenGLRenderDevice.h"

#include "glad/glad.h"

#include "OpenGLGraphicsShader.h"
#include "OpenGLComputeShader.h"

#include "Buffer/OpenGLUniformBuffer.h"
#include "Buffer/OpenGLStorageBuffer.h"

#include "OpenGLTexture.h"

namespace PathTracer {

	OpenGLRenderDevice::OpenGLRenderDevice(const RenderDeviceCreateInfo& createInfo) 
		: RenderDevice(createInfo) {
	}

	void OpenGLRenderDevice::ClearBuffer() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRenderDevice::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRenderDevice::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	std::shared_ptr<Texture> OpenGLRenderDevice::CreateTexture(const TextureCreateInfo& createInfo) {
		return m_Textures.emplace_back(std::make_shared<OpenGLTexture>(createInfo));
	}

	std::shared_ptr<OpenGLComputeShader> OpenGLRenderDevice::CreateComputeShader(const std::filesystem::path& path) {
		return std::make_shared<OpenGLComputeShader>(path);
	}

	std::shared_ptr<OpenGLGraphicsShader> OpenGLRenderDevice::CreateGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
		return std::make_shared<OpenGLGraphicsShader>(vertexPath, fragmentPath);
	}

	std::shared_ptr<StorageBuffer> OpenGLRenderDevice::CreateStorageBuffer(const std::string& name, uint32_t size) {
		return m_StorageBuffers.emplace_back(std::make_shared<OpenGLStorageBuffer>(name, size));
	}

	std::shared_ptr<UniformBuffer> OpenGLRenderDevice::CreateUniformBuffer(const std::string& name, uint32_t size) {
		return m_UniformBuffers.emplace_back(std::make_shared<OpenGLUniformBuffer>(name, size));
	}

	void OpenGLRenderDevice::CheckError(uint32_t id, uint32_t status) {
		GLint success;
		glGetProgramiv(id, status, &success);
		if (!success) {
			char infoLog[1024];
			glGetProgramInfoLog(id, 1024, nullptr, infoLog);
			PT_LOG_CRITICAL(infoLog);
		}
	}
}
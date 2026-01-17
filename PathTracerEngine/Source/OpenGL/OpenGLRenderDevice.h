#pragma once

#include "Core/Base.h"
#include "RenderDevice.h"

namespace PathTracer {

	class Texture;
	class OpenGLComputeShader;

	class OpenGLRenderDevice final : public RenderDevice, std::enable_shared_from_this<OpenGLRenderDevice> {
	public:
		OpenGLRenderDevice(const RenderDeviceCreateInfo& createInfo);
		virtual ~OpenGLRenderDevice() = default;

		void ClearBuffer() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetClearColor(float r, float g, float b, float a) override;

		std::shared_ptr<Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
		std::shared_ptr<OpenGLComputeShader> CreateComputeShader(const std::filesystem::path& path) override;
		std::shared_ptr<OpenGLGraphicsShader> CreateGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) override;
		std::shared_ptr<StorageBuffer> CreateStorageBuffer(const std::string& name, uint32_t size) override;
		std::shared_ptr<UniformBuffer> CreateUniformBuffer(const std::string& name, uint32_t size) override;
	private:
		void CheckError(uint32_t id, uint32_t status);
	}; 
}
#pragma once

#include "Texture.h"

namespace PathTracer {

	class Texture;
	class StorageBuffer;
	class UniformBuffer;
	class OpenGLComputeShader;
	class OpenGLGraphicsShader;

	struct RenderDeviceCreateInfo {

	};

	class RenderDevice {
	public:
		RenderDevice(const RenderDeviceCreateInfo& createInfo) 
			: m_CreateInfo(createInfo) {
		}
		virtual ~RenderDevice() = default;

		virtual void ClearBuffer() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;

		virtual std::shared_ptr<Texture> CreateTexture(const TextureCreateInfo& createInfo) = 0;
		virtual std::shared_ptr<OpenGLComputeShader> CreateComputeShader(const std::filesystem::path& path) = 0;
		virtual std::shared_ptr<OpenGLGraphicsShader> CreateGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) = 0;
		virtual std::shared_ptr<StorageBuffer> CreateStorageBuffer(const std::string& name, uint32_t size) = 0;
		virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(const std::string& name, uint32_t size) = 0;

		virtual void Initialize() {};

		template<typename CmdVec, typename BindFn>
		void BindUniformBuffers(const CmdVec& cmdVec, BindFn&& bindFn) {
			BindBuffers(cmdVec, m_UniformBuffers, std::forward<BindFn>(bindFn));
		}
		
		template<typename CmdVec, typename BindFn>
		void BindImages(const CmdVec& cmdVec, BindFn&& bindFn) {
			for (const auto& cmdBinding : cmdVec) {
				bindFn(cmdBinding.Texture, cmdBinding);
			}
		}
		
		template<typename CmdVec, typename BindFn>
		void BindStorageBuffers(const CmdVec& cmdVec, BindFn&& bindFn) {
			BindBuffers(cmdVec, m_StorageBuffers, std::forward<BindFn>(bindFn));
		}
	protected:
		inline const RenderDeviceCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

		template<typename CmdVec, typename BufferVec, typename BindFn>
		void BindBuffers(const CmdVec& cmdVec, const BufferVec& bufferVec, BindFn&& bindFn) {
			for (const auto& cmdBinding : cmdVec) {
				auto it = std::find_if(bufferVec.begin(), bufferVec.end(), [&](const auto& buffer) {
					return buffer->GetName() == cmdBinding.Name;
				});
				if (it != bufferVec.end()) {
					bindFn(**it, cmdBinding);
				}
			}
		}

		std::vector<std::shared_ptr<Texture>> m_Textures;

		std::vector<std::shared_ptr<UniformBuffer>> m_UniformBuffers;
		std::vector<std::shared_ptr<StorageBuffer>> m_StorageBuffers;
	private:
		RenderDeviceCreateInfo m_CreateInfo;
	};
}
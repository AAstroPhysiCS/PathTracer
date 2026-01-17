#pragma once

#include "Core/Base.h"

#include "OpenGLRenderDevice.h"

namespace PathTracer {

	class Texture;
	enum class TextureAccess : uint8_t;

	class OpenGLComputeShader {
	public:
		OpenGLComputeShader(const std::filesystem::path& path);
		virtual ~OpenGLComputeShader() = default;

		void Bind() const;
		void Unbind() const;
		void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) const;

		void BindImage(const std::shared_ptr<Texture>& textureToBind, TextureAccess access);
	private:
		uint32_t m_ShaderID;

		std::shared_ptr<OpenGLRenderDevice> m_RenderDevice;
	};
}
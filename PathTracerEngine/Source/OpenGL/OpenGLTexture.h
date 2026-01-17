#pragma once

#include "Texture.h"

#include "glad/glad.h"

namespace PathTracer {

	static GLenum ToGLWrap(TextureWrap wrap);
	static GLenum ToGLFilter(TextureFilter filter);

	struct OpenGLTextureFormat {
		GLenum InternalFormat;
		GLenum Format;
		GLenum Type;
	};

	OpenGLTextureFormat ToGLTextureFormat(TextureFormat format);
	GLenum ToGLAccess(TextureAccess access);

	class OpenGLTexture final : public Texture {
	public:
		OpenGLTexture(const TextureCreateInfo& createInfo);
		virtual ~OpenGLTexture() = default;

		void Bind() override;
		void Unbind() override;
		void Resize(uint32_t newWidth, uint32_t newHeight) override;
	private:
		void CreateEmptyTexture();
		void LoadTextureFromPath();
	};
}
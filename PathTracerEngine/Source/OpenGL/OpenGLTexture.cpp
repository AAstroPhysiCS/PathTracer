#include "OpenGLTexture.h"

#include "Core/Base.h"

#include "glad/glad.h"

#include "stb/stb_image.h"

namespace PathTracer {

	OpenGLTexture::OpenGLTexture(const TextureCreateInfo& createInfo) 
		: Texture(createInfo) {
		TextureID texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		SetID(texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLWrap(createInfo.Sampler.WrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLWrap(createInfo.Sampler.WrapV));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLFilter(createInfo.Sampler.MinFilter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLFilter(createInfo.Sampler.MagFilter));

		if (createInfo.Path.empty()) {
			CreateEmptyTexture();
			return;
		}

		LoadTextureFromPath();

		m_Handle = glGetTextureHandleARB(texture);
		glMakeTextureHandleResidentARB(m_Handle);
	}
	
	void OpenGLTexture::Bind() {
		glBindTexture(GL_TEXTURE_2D, GetID());
	}

	void OpenGLTexture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::CreateEmptyTexture() {
		const auto& createInfo = GetCreateInfo();
		auto format = ToGLTextureFormat(createInfo.Format);

		glTexImage2D(GL_TEXTURE_2D, 0, format.InternalFormat, createInfo.Width, createInfo.Height, 0, format.Format, format.Type, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLTexture::LoadTextureFromPath() {
		auto& createInfo = GetCreateInfo();
		auto format = ToGLTextureFormat(createInfo.Format);

		int width, height, channels;

		stbi_set_flip_vertically_on_load(false);

		auto GetSTBIChannelsForFormat = [](TextureFormat fmt) -> int {
			switch (fmt) {
				case TextureFormat::R8_UNorm:     return 1;
				case TextureFormat::RG8_UNorm:    return 2;
				case TextureFormat::RGB8_UNorm:   return 3;
				case TextureFormat::RGBA8_UNorm:  return 4;
				case TextureFormat::SRGB8:        return 3;
				case TextureFormat::SRGBA8:       return 4;
				default:                          return 4; // float/HDR fallback
			}
		};

		int forceChannels = GetSTBIChannelsForFormat(createInfo.Format);

		if (format.Type == GL_FLOAT || format.Type == GL_HALF_FLOAT) {
			// HDR / float textures
			float* data = stbi_loadf(createInfo.Path.string().c_str(), &width, &height, &channels, forceChannels);
			PT_ASSERT(data, std::format("Failed to load HDR texture: %s", createInfo.Path.string().c_str()));

			glTexImage2D(GL_TEXTURE_2D, 0, format.InternalFormat,
				width, height, 0, format.Format, format.Type, data);

			stbi_image_free(data);
		} else {
			// LDR / 8-bit textures
			uint8_t* data = stbi_load(createInfo.Path.string().c_str(), &width, &height, &channels, forceChannels);
			PT_ASSERT(data, std::format("Failed to load texture: %s", createInfo.Path.string().c_str()));

			glTexImage2D(GL_TEXTURE_2D, 0, format.InternalFormat,
				width, height, 0, format.Format, format.Type, data);

			stbi_image_free(data);
		}

		createInfo.Width = width;
		createInfo.Height = height;

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLTexture::Resize(uint32_t newWidth, uint32_t newHeight) {
		auto& createInfo = GetCreateInfo();

		createInfo.Width = newWidth;
		createInfo.Height = newHeight;

		auto format = ToGLTextureFormat(createInfo.Format);

		glBindTexture(GL_TEXTURE_2D, GetID());
		glTexImage2D(GL_TEXTURE_2D, 0, format.InternalFormat, createInfo.Width, createInfo.Height, 0, format.Format, format.Type, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	static GLenum ToGLWrap(TextureWrap wrap) {
		switch (wrap) {
			case TextureWrap::Repeat:         return GL_REPEAT;
			case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
			case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
		}
		return GL_REPEAT;
	}

	static GLenum ToGLFilter(TextureFilter filter) {
		switch (filter) {
			case TextureFilter::Nearest:                return GL_NEAREST;
			case TextureFilter::Linear:                 return GL_LINEAR;
			case TextureFilter::NearestMipmapNearest:   return GL_NEAREST_MIPMAP_NEAREST;
			case TextureFilter::LinearMipmapNearest:    return GL_LINEAR_MIPMAP_NEAREST;
			case TextureFilter::NearestMipmapLinear:    return GL_NEAREST_MIPMAP_LINEAR;
			case TextureFilter::LinearMipmapLinear:     return GL_LINEAR_MIPMAP_LINEAR;
		}
		return GL_LINEAR;
	}

	GLenum ToGLAccess(TextureAccess access) {
		switch (access) {
			case TextureAccess::ReadOnly:  return GL_READ_ONLY;
			case TextureAccess::WriteOnly: return GL_WRITE_ONLY;
			case TextureAccess::ReadWrite: return GL_READ_WRITE;
		}
		return GL_READ_WRITE;
	}

	OpenGLTextureFormat ToGLTextureFormat(TextureFormat format) {
		switch (format) {
			case TextureFormat::R8_UNorm:     return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
			case TextureFormat::RG8_UNorm:    return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
			case TextureFormat::RGB8_UNorm:   return { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };
			case TextureFormat::RGBA8_UNorm:  return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
			case TextureFormat::R16_Float:    return { GL_R16F, GL_RED, GL_HALF_FLOAT };
			case TextureFormat::RG16_Float:   return { GL_RG16F, GL_RG, GL_HALF_FLOAT };
			case TextureFormat::RGBA16_Float: return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT };
			case TextureFormat::R32_Float:    return { GL_R32F, GL_RED, GL_FLOAT };
			case TextureFormat::RG32_Float:   return { GL_RG32F, GL_RG, GL_FLOAT };
			case TextureFormat::RGBA32_Float: return { GL_RGBA32F, GL_RGBA, GL_FLOAT };
			case TextureFormat::R32_Int:      return { GL_R32I, GL_RED_INTEGER, GL_INT };
			case TextureFormat::RG32_Int:     return { GL_RG32I, GL_RG_INTEGER, GL_INT };
			case TextureFormat::RGBA32_Int:   return { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT };
			case TextureFormat::Depth16:      return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };
			case TextureFormat::Depth24Stencil8: return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
			case TextureFormat::Depth32_Float:return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
			case TextureFormat::SRGB8:        return { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE };
			case TextureFormat::SRGBA8:       return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE };
			default:
				PT_ASSERT(false, "Unknown TextureFormat");
		}
		return {};
	}
}

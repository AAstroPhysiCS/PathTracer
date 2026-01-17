#pragma once

#include "Core/Base.h"

namespace PathTracer {

	enum class TextureWrap {
		Repeat,
		ClampToEdge,
		MirroredRepeat
	};

	enum class TextureFilter {
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	struct TextureSampler {
		TextureWrap WrapU = TextureWrap::Repeat;
		TextureWrap WrapV = TextureWrap::Repeat;
		TextureWrap WrapW = TextureWrap::Repeat;

		TextureFilter MinFilter = TextureFilter::LinearMipmapLinear;
		TextureFilter MagFilter = TextureFilter::Linear;
	};

	enum class TextureFormat : uint8_t {
		None = 0,

		// 8-bit normalized
		R8_UNorm,
		RG8_UNorm,
		RGB8_UNorm,
		RGBA8_UNorm,

		// 16-bit
		R16_Float,
		RG16_Float,
		RGBA16_Float,

		// 32-bit float
		R32_Float,
		RG32_Float,
		RGBA32_Float,

		// Integer formats
		R32_Int,
		RG32_Int,
		RGBA32_Int,

		// Depth / Stencil
		Depth16,
		Depth24Stencil8,
		Depth32_Float,

		// sRGB
		SRGB8,
		SRGBA8
	};

	struct TextureCreateInfo {
		//if no path is specified, then create a blank texture
		std::filesystem::path Path;
		uint32_t Width;
		uint32_t Height;
		TextureFormat Format;
		TextureSampler Sampler;
	};

	enum class TextureAccess : uint8_t {
		None,
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	class Texture {
	public:
		using TextureID = uint32_t;

		Texture(const TextureCreateInfo& createInfo) 
			: m_CreateInfo(createInfo) {
		}
		virtual ~Texture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t newWidth, uint32_t newHeight) = 0;

		inline TextureID GetID() const { return m_ID; }
		inline TextureID GetHandle() const { return m_Handle; }

		inline uint32_t GetWidth() { return GetCreateInfo().Width; }
		inline uint32_t GetHeight() { return GetCreateInfo().Height; }
		inline std::filesystem::path GetPath() { return GetCreateInfo().Path; }

		inline TextureFormat GetFormat() { return GetCreateInfo().Format; }
		inline TextureSampler GetSampler() { return GetCreateInfo().Sampler; }
	protected:
		inline TextureCreateInfo& GetCreateInfo() { return m_CreateInfo; }

		inline void SetID(TextureID id) { m_ID = id; }

		TextureID m_Handle = 0;
	private:
		TextureCreateInfo m_CreateInfo;

		TextureID m_ID = 0;
	};
}


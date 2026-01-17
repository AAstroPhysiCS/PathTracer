#pragma once
#include "Renderer.h"

namespace PathTracer {

    class OpenGLStorageBuffer;
    class OpenGLUniformBuffer;

	class OpenGLComputeShader;
	class OpenGLGraphicsShader;
	class Texture;

    enum class MemoryBarrier : uint32_t {
        VertexAttrib = 0x00000001, // GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
        ElementArray = 0x00000002, // GL_ELEMENT_ARRAY_BARRIER_BIT
        Uniform = 0x00000004, // GL_UNIFORM_BARRIER_BIT
        TextureFetch = 0x00000008, // GL_TEXTURE_FETCH_BARRIER_BIT
        ShaderImage = 0x00000020, // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
        Command = 0x00000040, // GL_COMMAND_BARRIER_BIT
        PixelBuffer = 0x00000080, // GL_PIXEL_BUFFER_BARRIER_BIT
        TextureUpdate = 0x00000100, // GL_TEXTURE_UPDATE_BARRIER_BIT
        BufferUpdate = 0x00000200, // GL_BUFFER_UPDATE_BARRIER_BIT
        Framebuffer = 0x00000400, // GL_FRAMEBUFFER_BARRIER_BIT
        TransformFeedback = 0x00000800, // GL_TRANSFORM_FEEDBACK_BARRIER_BIT
        AtomicCounter = 0x00001000, // GL_ATOMIC_COUNTER_BARRIER_BIT
        ShaderStorage = 0x00002000, // GL_SHADER_STORAGE_BARRIER_BIT
        AllMemory = 0xFFFFFFFF  // GL_ALL_BARRIER_BITS
    };

    inline MemoryBarrier operator|(MemoryBarrier a, MemoryBarrier b) {
        return static_cast<MemoryBarrier>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline MemoryBarrier& operator|=(MemoryBarrier& a, MemoryBarrier b) {
        a = a | b;
        return a;
    }

	class OpenGLRenderer final : public Renderer {
	public:
		OpenGLRenderer(const RendererCreateInfo& createInfo);
		virtual ~OpenGLRenderer() = default;

        void Initialize() override;

        void DispatchComputeDrawCommand(const ComputeDrawCommand& cmd) override;
		void SetMemoryBarrier(MemoryBarrier barrier);

        void BeginFrame() override;
        void RenderToSwapChain(const std::shared_ptr<Texture>& input) override;
		void EndFrame() override;

		void Resize(uint32_t newWidth, uint32_t newHeight) override;

		void Shutdown() override;
    private:
        std::shared_ptr<OpenGLGraphicsShader> m_FullscreenShader;

        uint32_t m_FullscrenVAO = 0;
	};
}
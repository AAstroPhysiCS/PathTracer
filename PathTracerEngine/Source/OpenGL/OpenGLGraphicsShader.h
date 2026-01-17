#pragma once
#include "Core/Base.h"
#include <cstdint>

namespace PathTracer {

    class Texture;

    class OpenGLGraphicsShader {
    public:
        OpenGLGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        virtual ~OpenGLGraphicsShader() = default;

        void Bind() const;
        void Unbind() const;

        void BindImage(const std::string& name, const std::shared_ptr<Texture>& input, uint32_t binding) const;

        void SetBool(const std::string& name, bool value) const;
        void SetFloat(const std::string& name, float value) const;
    private:
        uint32_t m_ShaderID = 0;
    };
}
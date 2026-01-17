#pragma once

#include "UniformBuffer.h"

#include <glad/glad.h>

namespace PathTracer {

	class OpenGLUniformBuffer final : public UniformBuffer {
	public:
		OpenGLUniformBuffer(const std::string& name, uint32_t size);
		~OpenGLUniformBuffer() override;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		void Bind(uint32_t binding) const override;

		const std::string& GetName() const override { return m_Name; }
	private:
		std::string m_Name;
		uint32_t m_Size = 0;
		GLuint m_ID = 0;
	};

}

#include "OpenGLUniformBuffer.h"

namespace PathTracer {

	OpenGLUniformBuffer::OpenGLUniformBuffer(const std::string& name, uint32_t size)
		: m_Name(name), m_Size(size) {

		glCreateBuffers(1, &m_ID);
		glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer() {
		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
		glNamedBufferSubData(m_ID, offset, size, data);
	}

	void OpenGLUniformBuffer::Bind(uint32_t binding) const {
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_ID);
	}
}

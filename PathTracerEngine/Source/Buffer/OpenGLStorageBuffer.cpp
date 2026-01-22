#include "OpenGLStorageBuffer.h"

#include "Core/Base.h"

namespace PathTracer {

	OpenGLStorageBuffer::OpenGLStorageBuffer(const std::string& name, uint32_t size)
		: m_Name(name), m_Size(size) {
		glCreateBuffers(1, &m_ID);

		const GLbitfield flags =
			GL_MAP_WRITE_BIT |
			GL_MAP_PERSISTENT_BIT |
			GL_MAP_COHERENT_BIT;

		glNamedBufferStorage(m_ID, size, nullptr, flags);
		m_MappedPtr = glMapNamedBufferRange(m_ID, 0, size, flags);
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer() {
		if (m_MappedPtr)
			glUnmapNamedBuffer(m_ID);

		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLStorageBuffer::SetData(const void* data, size_t size, uint32_t offset) {
		if (!m_MappedPtr)
			return;

		if (offset + size > m_Size) {
			PT_LOG_CRITICAL(std::format("{0}: OpenGLStorageBuffer: Size is greater than the available size!", m_Name));
			return;
		}

		std::memcpy(static_cast<uint8_t*>(m_MappedPtr) + offset, data, size);
	}

	size_t OpenGLStorageBuffer::AppendData(const void* data, size_t size) {
		if (m_Head + size > m_Size) {
			PT_LOG_CRITICAL(std::format("{0}: OpenGLStorageBuffer: Out of space!", m_Name));
			return UINT32_MAX;
		}

		size_t offset = m_Head;
		std::memcpy(static_cast<uint8_t*>(m_MappedPtr) + m_Head, data, size);

		m_Head += size;
		return offset;
	}

	void OpenGLStorageBuffer::Bind(uint32_t binding) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ID);
	}

	void OpenGLStorageBuffer::Clear() {
		PT_LOG_INFO("TODO:");
	}
}

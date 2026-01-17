#pragma once

#include "StorageBuffer.h"

#include <glad/glad.h>

namespace PathTracer {

	class OpenGLStorageBuffer final : public StorageBuffer {
	public:
		OpenGLStorageBuffer(const std::string& name, uint32_t size);
		~OpenGLStorageBuffer() override;

		void SetData(const void* data, size_t size, uint32_t offset = 0) override;
		size_t AppendData(const void* data, size_t size) override;
		void Bind(uint32_t binding) const override;
		void Clear() override;

		size_t GetSize() const { return m_Size; }
		size_t GetUsedSize() const { return m_Head; }

		const std::string& GetName() const override { return m_Name; }
	private:
		void* m_MappedPtr = nullptr;

		std::string m_Name;
		size_t m_Size = 0;
		size_t m_Head = 0;
		GLuint m_ID = 0;
	};
}

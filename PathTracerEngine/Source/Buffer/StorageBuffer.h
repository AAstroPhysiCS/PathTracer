#pragma once

#include <cstdint>
#include <string>

namespace PathTracer {

	class StorageBuffer {
	public:
		virtual ~StorageBuffer() = default;

		virtual void SetData(const void* data, size_t size, uint32_t offset = 0) = 0;
		virtual size_t AppendData(const void* data, size_t size) = 0;
		virtual void Bind(uint32_t binding) const = 0;
		virtual void Clear() = 0;

		virtual const std::string& GetName() const = 0;
	};
}

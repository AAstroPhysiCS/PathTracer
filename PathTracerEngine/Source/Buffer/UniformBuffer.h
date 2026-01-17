#pragma once

#include <cstdint>
#include <string>

namespace PathTracer {

	class Shader;

	class UniformBuffer {
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind(uint32_t binding) const = 0;

		virtual const std::string& GetName() const = 0;
	};
}

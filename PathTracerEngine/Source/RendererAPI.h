#pragma once
#include <cstdint>

enum class RendererAPI : uint8_t {
	OpenGL = 0,
	//DirectX = 1,
	//Vulkan = 2 maybe in the future with hardware-accelerated ray tracing
};
#pragma once

#include "glm/glm.hpp"

namespace PathTracer {

	static inline constexpr const glm::vec4 SUN_COLOR_MIDDAY = glm::vec4(1.0f, 1.0f, 0.98f, 1.0f);
	
	struct alignas(16) DirectionalLight final {
		glm::vec4 Direction{ 0.0f, 90.0f, 0.0f, 1.0f};
		glm::vec4 Color{ SUN_COLOR_MIDDAY };
		float Intensity = 20.0f;
	};

	//TODO: other type of lights etc...
}

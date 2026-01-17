#pragma once

#include "Core/Base.h"

#include "OpenGL/OpenGLComputeShader.h"

namespace PathTracer {

	struct ComputeUniformBinding {
		std::string_view Name;
		uint32_t Binding = 0;

		std::shared_ptr<UniformBuffer> Handle;
	};

	struct ComputeSSBOBinding {
		std::string_view Name;
		uint32_t Binding = 0;

		std::shared_ptr<StorageBuffer> Handle;
	};

	struct ComputeImageBinding {
		std::string_view Name;
		uint32_t Binding = 0;
		TextureAccess Access;

		std::shared_ptr<Texture> Texture;
	};

	struct NumWorkGroup {
		uint32_t X;
		uint32_t Y;
		uint32_t Z;
	};

	struct ComputeDrawCommand final {
		std::shared_ptr<OpenGLComputeShader> Shader; //in the future, abstract it in to ComputeShader
		std::vector<ComputeImageBinding> Images;
		std::vector<ComputeUniformBinding> Uniforms;
		std::vector<ComputeSSBOBinding> SSBOs;

		NumWorkGroup NumWorkGroup;
	};
}
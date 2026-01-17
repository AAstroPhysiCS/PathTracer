#include "OpenGLComputeShader.h"
#include "OpenGLTexture.h"
#include "glad/glad.h"
#include "OpenGLGraphicsShader.h"
#include "Core/Filesystem.h"

#include <ranges>
#include <numeric>
#include <set>

namespace PathTracer {

	OpenGLComputeShader::OpenGLComputeShader(const std::filesystem::path& path) {
		std::string computeSource = PathTracer::Filesystem::ReadFileAsString(path);

		std::set<std::filesystem::path> includeFiles; //to not include the same files over and over again
		const auto ReplaceAnyIncludeDirectives = [&](const std::string& source, const auto& SelfFunc) -> std::string {
			auto root = path.parent_path();
			auto lines = source | std::views::split('\n') | std::views::transform([&](const auto& subrange) {
				auto line = std::string(subrange.begin(), subrange.end());
				if (line.rfind("#include", 0) == 0) { // line starts with #include
					size_t startQuote = line.find('"');
					size_t endQuote = line.find('"', startQuote + 1);
					std::filesystem::path includeFile = root / line.substr(startQuote + 1, endQuote - startQuote - 1);
					if (startQuote != std::string::npos && endQuote != std::string::npos && !includeFiles.contains(includeFile)) {
						includeFiles.emplace(includeFile);
						return SelfFunc(PathTracer::Filesystem::ReadFileAsString(includeFile), SelfFunc);
					} else {
						line.replace(line.begin() + 1, line.end(), "");
					}
				}
				return line + "\n";
			});
			return std::accumulate(lines.begin(), lines.end(), std::string{});
		};

		computeSource = ReplaceAnyIncludeDirectives(computeSource, ReplaceAnyIncludeDirectives);

		const auto CheckError = [&](uint32_t id, uint32_t status) {
			GLint success;
			status == GL_LINK_STATUS ? glGetProgramiv(id, status, &success) : glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				status == GL_LINK_STATUS ? glGetProgramInfoLog(id, 1024, nullptr, infoLog) : glGetShaderInfoLog(id, 1024, nullptr, infoLog);
				PT_LOG_CRITICAL(infoLog);
			}
		};

		const auto CreateShader = [&](std::string_view computeShaderSource) {
			const char* sourceCStr = computeShaderSource.data();

			int32_t compute = glCreateShader(GL_COMPUTE_SHADER);
			glShaderSource(compute, 1, &sourceCStr, NULL);
			glCompileShader(compute);
			CheckError(compute, GL_COMPILE_STATUS);

			return compute;
		};

		const auto CreateProgram = [&](uint32_t shaderId) {
			GLuint program = glCreateProgram();
			glAttachShader(program, shaderId);
			glLinkProgram(program);

			CheckError(program, GL_LINK_STATUS);

			return program;
		};

		uint32_t shader = CreateShader(computeSource);
		m_ShaderID = CreateProgram(shader);

		glDeleteShader(shader);
	}

	void OpenGLComputeShader::Bind() const {
		glUseProgram(m_ShaderID);
	}

	void OpenGLComputeShader::Unbind() const {
		glUseProgram(0);
	}

	void OpenGLComputeShader::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) const {
		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	}

	void OpenGLComputeShader::BindImage(const std::shared_ptr<Texture>& textureToBind, TextureAccess access) {
		glBindImageTexture(0, textureToBind->GetID(), 0, GL_FALSE, 0, ToGLAccess(access), ToGLTextureFormat(textureToBind->GetFormat()).InternalFormat);
	}
}
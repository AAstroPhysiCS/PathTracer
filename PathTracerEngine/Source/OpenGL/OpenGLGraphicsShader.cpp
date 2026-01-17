#include "OpenGLGraphicsShader.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"

#include "Core/Filesystem.h"

namespace PathTracer {

	OpenGLGraphicsShader::OpenGLGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
		const std::string& vertexSource = PathTracer::Filesystem::ReadFileAsString(vertexPath);
		const std::string& fragmentSource = PathTracer::Filesystem::ReadFileAsString(fragmentPath);

		const auto CheckError = [&](uint32_t id, uint32_t status) {
			GLint success;
			status == GL_LINK_STATUS ? glGetProgramiv(id, status, &success) : glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				status == GL_LINK_STATUS ? glGetProgramInfoLog(id, 1024, nullptr, infoLog) : glGetShaderInfoLog(id, 1024, nullptr, infoLog);
				PT_LOG_CRITICAL(infoLog);
			}
		};

		const auto CreateShader = [&](std::string_view shaderSource, GLenum shaderType) {
			const char* sourceCStr = shaderSource.data();

			int32_t shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, &sourceCStr, NULL);
			glCompileShader(shader);
			CheckError(shader, GL_COMPILE_STATUS);

			return shader;
		};

		const auto CreateProgram = [&](uint32_t shaderIdVertex, uint32_t shaderIdFragment) {
			GLuint program = glCreateProgram();
			glAttachShader(program, shaderIdVertex);
			glAttachShader(program, shaderIdFragment);
			glLinkProgram(program);

			CheckError(program, GL_LINK_STATUS);

			return program;
		};

		uint32_t shaderVertex = CreateShader(vertexSource, GL_VERTEX_SHADER);
		uint32_t shaderFragment = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);
		m_ShaderID = CreateProgram(shaderVertex, shaderFragment);

		glDeleteShader(shaderVertex);
		glDeleteShader(shaderFragment);
	}

	void OpenGLGraphicsShader::Bind() const {
		glUseProgram(m_ShaderID);
	}

	void OpenGLGraphicsShader::Unbind() const {
		glUseProgram(0);
	}

	void OpenGLGraphicsShader::SetBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), (int)value);
	}

	void OpenGLGraphicsShader::BindImage(const std::string& name, const std::shared_ptr<Texture>& input, uint32_t binding) const {
		glActiveTexture(GL_TEXTURE0 + binding);
		glBindTexture(GL_TEXTURE_2D, input->GetID());
		glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), binding);
	}

	void OpenGLGraphicsShader::SetFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
	}
}

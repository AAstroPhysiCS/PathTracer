#pragma once

#include "Core/Base.h"
#include <filesystem>
#include <fstream>

namespace PathTracer::Filesystem {

	inline std::string ReadFileAsString(const std::filesystem::path& filepath) {
		std::ifstream file(filepath, std::ios::in | std::ios::binary);
		PT_ASSERT(file.is_open(), "Failed to open file: " + filepath.string());

		std::string content;
		file.seekg(0, std::ios::end);
		content.resize(file.tellg());
		file.seekg(0, std::ios::beg);

		file.read(&content[0], content.size());
		file.close();

		return content;
	}
}
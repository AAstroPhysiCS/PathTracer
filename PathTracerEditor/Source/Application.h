#pragma once
#include <string_view>
#include <cstdint>

#include "RendererAPI.h"

struct ApplicationCreateInfo {
	uint32_t Width;
	uint32_t Height;
	std::string_view Title;

	RendererAPI TargetAPI;
};

class Application {
public:
	Application(const ApplicationCreateInfo& createInfo)
		: m_CreateInfo(createInfo) {
	}
	virtual ~Application() = default;

	virtual void Initialize() = 0;
	virtual void Run() = 0;
	virtual int32_t Shutdown() = 0;

	uint32_t GetWidth() const { return m_CreateInfo.Width; }
	uint32_t GetHeight() const { return m_CreateInfo.Height; }
	std::string_view GetTitle() const { return m_CreateInfo.Title; }

	RendererAPI GetTargetAPI() const { return m_CreateInfo.TargetAPI; }
private:
	ApplicationCreateInfo m_CreateInfo;
};
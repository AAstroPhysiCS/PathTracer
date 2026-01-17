#pragma once

#include "Core/Base.h"

namespace PathTracer {

	class UIPanel {
	public:
		UIPanel() = default;
		virtual ~UIPanel() = default;

		virtual void Begin() const = 0;
		virtual void End() const = 0;
	};

	template <typename TPanel>
	concept IsPanel = std::is_base_of_v<UIPanel, TPanel>;
}


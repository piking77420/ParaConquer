#pragma once

#include <string>

#include <Imgui/imgui.h>

#include "math/toolbox_typedef.hpp"

namespace ImGui
{
	bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f);

	void TextCenter(const char* _text);

	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
}


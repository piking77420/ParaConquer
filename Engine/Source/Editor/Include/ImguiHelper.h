#pragma once

#include <String>

#include <imgui.h>

#include "Math/ToolboxTypedef.hpp"

namespace ImGui
{
    struct ScopedFont
    {
        ScopedFont(ImFont* font)
        {
            if (font)
                ImGui::PushFont(font);
        }
        ~ScopedFont()
        {
            ImGui::PopFont();
        }
    };

    bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f);

    void TextCenter(const char* _text);

    bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
}

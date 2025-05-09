﻿#include "imgui_helper.h"

#include "Imgui/imgui.h"


bool ButtonCenteredOnLine(const char* label, float alignment)
{
    
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);

    return false;
}

void TextCenter(const char* _text)
{
    
    float font_size = ImGui::GetFontSize() * strlen(_text) / 2;
    ImGui::SameLine(
        ImGui::GetWindowSize().x / 2 -
        font_size + (font_size / 2)
    );

    ImGui::Text(_text);
}

#include "ImguiHelper.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    bool ButtonCenteredOnLine(const char* label, float alignment)
    {
        ImGuiStyle& style = GetStyle();

        float size = CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        float avail = GetContentRegionAvail().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
            SetCursorPosX(GetCursorPosX() + off);

        return Button(label);

        return false;
    }

    void TextCenter(const char* _text)
    {
        float windowWidth = GetWindowSize().x;
        float textWidth = CalcTextSize(_text).x;
        float posX = (windowWidth - textWidth) * 0.5f;
        if (posX > 0.0f)
            SetCursorPosX(posX);
        Text("%s", _text);
    }

    //https://github.com/ocornut/imgui/issues/1901
    bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;

        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

        const float t = g.Time;
        const float r = size.y / 2;
        constexpr float speed = 1.5f;

        const float a = speed * 0;
        const float b = speed * 0.333f;
        const float c = speed * 0.666f;

        const float o1 = (circleWidth + r) * (t + a - speed * static_cast<int>((t + a) / speed)) / speed;
        const float o2 = (circleWidth + r) * (t + b - speed * static_cast<int>((t + b) / speed)) / speed;
        const float o3 = (circleWidth + r) * (t + c - speed * static_cast<int>((t + c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);

        return true;
    }

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * static_cast<float>(start) / static_cast<float>(num_segments);
        const float a_max = IM_PI * 2.0f * (static_cast<float>(num_segments) - 3) / static_cast<float>(num_segments);

        const auto centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++)
        {
            const float a = a_min + (static_cast<float>(i) / static_cast<float>(num_segments)) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
                                                centre.y + ImSin(a + g.Time * 8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
        return true;
    }
}

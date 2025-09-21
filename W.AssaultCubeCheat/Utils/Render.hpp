#pragma once
#include <string>

#include "../Cheat.h"
#include "../ImGui/imgui.h"

#define COLOR_RED       ImColor(255, 0, 0)
#define COLOR_GREEN     ImColor(0, 255, 0)
#define COLOR_BLUE      ImColor(0, 0, 255)
#define COLOR_WHITE     ImColor(255, 255, 255)
#define COLOR_BLACK     ImColor(0, 0, 0)
#define COLOR_YELLOW    ImColor(255, 255, 0)
#define COLOR_ORANGE    ImColor(255, 165, 0)
#define COLOR_PURPLE    ImColor(128, 0, 128)
#define COLOR_CYAN      ImColor(0, 255, 255)
#define COLOR_MAGENTA   ImColor(255, 0, 255)

#define COLOR_GREY      ImColor(128, 128, 128)
#define COLOR_DARKGREY  ImColor(64, 64, 64)
#define COLOR_LIGHTGREY ImColor(192, 192, 192)

#define COLOR_PINK      ImColor(255, 105, 180)
#define COLOR_BROWN     ImColor(139, 69, 19)
#define COLOR_GOLD      ImColor(255, 215, 0)
#define COLOR_SILVER    ImColor(192, 192, 192)
#define COLOR_LIME      ImColor(50, 205, 50)
#define COLOR_TURQUOISE ImColor(64, 224, 208)
#define COLOR_NAVY      ImColor(0, 0, 128)
#define COLOR_MAROON    ImColor(128, 0, 0)


class Render
{
public:
    // Simple filled rectangle (2D)
    inline static void Box(Vec2 position, Vec2 size, ImColor color, float thickness = 1.0f)
    {
        //TL -> TR
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(position.x, position.y),
            ImVec2(position.x + size.x, position.y),
            color,
            thickness
        );

        //TR -> BR
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(position.x + size.x, position.y),
            ImVec2(position.x + size.x, position.y + size.y),
            color,
            thickness
        );

        //BR->BL
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(position.x + size.x, position.y + size.y),
            ImVec2(position.x, position.y + size.y),
            color,
            thickness
        );

        //BL->TL
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(position.x, position.y + size.y),
            ImVec2(position.x, position.y),
            color,
            thickness
        );
    }

    inline static void BoxFilled(Vec2 position, Vec2 size, ImColor color, float rounding = 0.0f)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(position.x, position.y),
            ImVec2(position.x + size.x, position.y + size.y),
            color,
            rounding
        );
    }

    inline static void Text(Vec2 position, ImColor color, std::string text, float fontSize)
    {
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, position.ToImVec2(), color, text.c_str());
    }

    // Line between two points
    inline static void Line(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f)
    {
	    
    }

    // Circle (for FOV etc.)
    inline static void Circle(float x, float y, float radius, ImU32 color, int segments = 32, float thickness = 1.0f)
    {
	    
    }
};

#include "utils.h"

#include "raymath.h"

bool IsPointInRect(const Vector2& pointPos, const Vector2& rectPos, const Vector2& rectSize) {
    float rectLeft = rectPos.x;
    float rectRight = rectPos.x + rectSize.x;
    float rectTop = rectPos.y;
    float rectBottom = rectPos.y + rectSize.y;

    return pointPos.x >= rectLeft && pointPos.x <= rectRight && pointPos.y >= rectTop && pointPos.y <= rectBottom;
}

bool IsPointInCircle(const Vector2& pointPos, const Vector2& circleCenter, float circleRadius) {
    return Vector2Distance(pointPos, circleCenter) < circleRadius;
}

void DrawTextCenter(const Font& font, const std::string& msg, const Vector2& pos, Color color) {
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(font, msg.c_str(), font.baseSize, spacing);
    DrawTextPro(font, msg.c_str(), pos, { textSize.x / 2, textSize.y / 2 }, 0.0f, font.baseSize, spacing, color);
}

void DrawTextRightAlign(const Font& font, const std::string& msg, const Vector2& pos, Color color) {
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(font, msg.c_str(), font.baseSize, spacing);
    DrawTextPro(font, msg.c_str(), { pos.x - textSize.x / 2, pos.y }, { textSize.x / 2, textSize.y / 2 }, 0.0f, font.baseSize, spacing, color);
}
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

bool DrawTextAnchor(const Font& font, const std::string& msg, const Vector2& pos, int anchorX, int anchorY, Color color) {
    if (anchorX > 1 || anchorX < -1 || anchorY > 1 || anchorY < -1) return false;

    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(font, msg.c_str(), font.baseSize, spacing);
    Vector2 textPos = { pos.x + anchorX * textSize.x / 2, pos.y + anchorY * textSize.y / 2 };
    DrawTextPro(font, msg.c_str(), textPos, { textSize.x / 2, textSize.y / 2 }, 0.0f, font.baseSize, spacing, color);
    return true;
}

void DrawTextCenter(const Font& font, const std::string& msg, const Vector2& pos, Color color) {
    DrawTextAnchor(font, msg, pos, 0, 0, color);
}

void DrawTextRightAlign(const Font& font, const std::string& msg, const Vector2& pos, Color color) {
    DrawTextAnchor(font, msg, pos, -1, 0, color);
}

void DrawTextTopCenter(const Font& font, const std::string& msg, const Vector2& pos, Color color) {
    DrawTextAnchor(font, msg, pos, 0, 1, color);
}
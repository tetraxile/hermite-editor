#pragma once

#include <string>

#include "raylib.h"
#include "rcamera.h"

template <typename... Args>
inline std::string format(const char* fmt, Args... args) {
    int len = snprintf(nullptr, 0, fmt, args...);
    std::string str(len + 1, '\0');
    snprintf(str.data(), len + 1, fmt, args...);
    return str;
}

bool IsPointInRect(const Vector2& pointPos, const Vector2& rectPos, const Vector2& rectSize);

bool IsPointInCircle(const Vector2& pointPos, const Vector2& circleCenter, float circleRadius);

void DrawTextCenter(const Font& font, const std::string& msg, const Vector2& pos, Color color);

void DrawTextRightAlign(const Font& font, const std::string& msg, const Vector2& pos, Color color);
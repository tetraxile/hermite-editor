#pragma once

#include <vector>

#include "raylib.h"
#include "rcamera.h"

struct KeyFrame;

struct Graph {
    Graph(const Font& font) : font(font) {}

    const Vector2 coordToScreenPos(const Vector2& coord) const {
        float xPos = left + coord.x * frameWidth;
        float yPos = bottom - coord.y * height / yRange;
        return { xPos, yPos };
    }

    const Vector2 screenPosToCoord(const Vector2& screenPos) const {
        float xCoord = (screenPos.x - left) / frameWidth;
        float yCoord = (bottom - screenPos.y) / height * yRange;
        return { xCoord, yCoord };
    }

    void draw(const std::vector<KeyFrame>& keyframes) const;

    const Font& font;

    const float top = 100.0f;
    const float bottom = 700.0f;
    const float left = 100.0f;
    const float right = 1500.0f;
    const float yRange = 2.0f;

    const float width = right - left;
    const float height = bottom - top;

    const int yTickCount = 20;
    const float yTickHeight = height / yTickCount;
    
    const Vector2 topLeft = { left, top };
    const Vector2 size = { width, height };
    
    int frameCount = 30;
    float frameWidth = width / frameCount;
};
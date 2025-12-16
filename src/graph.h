#pragma once

#include <vector>

#include "raylib.h"
#include "rcamera.h"

struct KeyFrame;

struct Graph {
    Graph(const Font& font, int screenWidth, int screenHeight) : font(font), bottom(screenHeight - 200.0f), right(screenWidth - 100.0f) {}

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

    void resize(int screenWidth, int screenHeight) {
        bottom = screenHeight - 200.0f;
        right = screenWidth - 100.0f;
        width = right - left;
        height = bottom - top;
        yTickHeight = height / yTickCount;
        size = { width, height };
        frameWidth = width / frameCount;
    }

    void draw(const std::vector<KeyFrame>& keyframes) const;

    const Font& font;

    const float top = 100.0f;
    const float left = 100.0f;
    float bottom;
    float right;
    const float yRange = 2.0f;

    float width = right - left;
    float height = bottom - top;

    const int yTickCount = 20;
    float yTickHeight = height / yTickCount;
    
    const Vector2 topLeft = { left, top };
    Vector2 size = { width, height };
    
    int frameCount = 30;
    float frameWidth = width / frameCount;
};
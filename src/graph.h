#pragma once

#include "keyframe.h"

#include <vector>

#include "raylib.h"
#include "rcamera.h"

struct Graph {
    Graph(const Font& font, int screenWidth, int screenHeight);

    const Vector2 coordToScreenPos(const Vector2& coord) const;
    const Vector2 screenPosToCoord(const Vector2& screenPos) const;

    void resize(int screenWidth, int screenHeight);
    void draw() const;
    void drawKeyframes() const;
    void update(const Vector2& mousePos);

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

    std::vector<KeyFrame> keyframes;
    KeyFrame* selectedKeyframe = nullptr;
    KeyFrame* draggingKeyframe = nullptr;
    float keyframePixelsMoved = 0.0f;
    KeyFrame::Control selectedKeyframeControl = KeyFrame::Control::NONE;

    Vector2 ghostKeyframePos;
    bool isClickingNewKeyframe = false;
};
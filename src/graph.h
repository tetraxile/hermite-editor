#pragma once

#include "utils.h"

#include "raylib.h"
#include "rcamera.h"

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

    void draw() const {
        // draw timeline (x-axis)
        DrawLineEx({ left, bottom }, { right, bottom }, 3, BLACK);

        // draw y-axis
        DrawLineEx({ left, top }, { left, bottom }, 3, BLACK);

        // draw grid
        for (int i = 1; i <= frameCount; i++) {
            float linePosX = left + i * frameWidth;
            DrawLineV({ linePosX, bottom }, { linePosX, top }, i % 5 == 0 ? GRAY : LIGHTGRAY);
        }
        for (int i = 1; i <= yTickCount; i++) {
            float linePosY = bottom - i * yTickHeight;
            DrawLineV({ left, linePosY }, { right, linePosY }, i % 5 == 0 ? GRAY : LIGHTGRAY);
        }

        // draw ticks under timeline
        for (int i = 0; i <= frameCount; i++) {
            if (frameCount > 45 && i % 2 == 1) continue;
            if (frameCount > 90 && i % 4 == 2) continue;
            float tickPosX = left + i * frameWidth;
            DrawLineEx({ tickPosX, bottom }, { tickPosX, bottom + 10 }, 2, GRAY);
            DrawTextCenter(font, format("%d", i), { tickPosX, bottom + 25 }, BLACK);
        }

        // draw ticks next to y-axis
        for (int i = 0; i <= yTickCount; i++) {
            float linePosY = bottom - i * yTickHeight;
            DrawLineEx({ left, linePosY }, { left - 10, linePosY }, 2, GRAY);
            float tickValue = yRange * i / yTickCount;
            DrawTextRightAlign(font, format("%.1f", tickValue), { left - 20, linePosY }, BLACK);
        }
    }

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
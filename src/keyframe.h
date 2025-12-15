#pragma once

#include "graph.h"

#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include "utils.h"

struct KeyFrame {
    enum class Control {
        NONE,
        LEFT,
        RIGHT,
    };

    static inline constexpr float RADIUS = 5.0f;

    KeyFrame(int frame, float value, float slope) : frame(frame), value(value), slope(slope) {}

    int getFrame() const {
        return frame;
    }

    const Vector2 getScreenPos(const Graph& graph) const {
        return graph.coordToScreenPos(getPos());
    }

    bool isClicked(const Graph& graph, const Vector2& mousePos) const {
        Vector2 screenPos = getScreenPos(graph);
        return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && frame <= graph.frameCount && IsPointInCircle(mousePos, getScreenPos(graph), RADIUS);
    }

    const Vector2 getSlopeControlPos(const Graph& graph, Control control) const {
        Vector2 screenPos = getScreenPos(graph);
        Vector2 slopeDir = {1.0f, slope};
        Vector2 delta = Vector2Normalize(graph.coordToScreenPos(getPos() - slopeDir) - screenPos) * graph.frameWidth;
        if (control == Control::LEFT)
            return screenPos + delta;
        else if (control == Control::RIGHT)
            return screenPos - delta;
        else
            return { 0.0f, 0.0f };
    }

    void draw(const Graph& graph, bool isSelected) const {
        Vector2 screenPos = getScreenPos(graph);

        if (frame > graph.frameCount) return;

        DrawCircleV(screenPos, RADIUS, isSelected ? ORANGE : RED);

        if (isSelected) {
            Vector2 leftControlPos = getSlopeControlPos(graph, Control::LEFT);
            Vector2 rightControlPos = getSlopeControlPos(graph, Control::RIGHT);
            DrawLineEx(leftControlPos, rightControlPos, 2, RED);

            DrawCircleV(leftControlPos, RADIUS, BLUE);
            DrawCircleV(rightControlPos, RADIUS, BLUE);
        }
    }
    
    int frame;
    float value;
    float slope;
private:
    const Vector2 getPos() const {
        return { (float)frame, value };
    }
};
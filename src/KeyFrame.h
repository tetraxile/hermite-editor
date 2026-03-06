#pragma once

#include "raylib.h"
#include "rcamera.h"

struct Graph;

struct KeyFrame {
    enum class Control {
        NONE,
        LEFT,
        RIGHT,
    };

    static inline constexpr float RADIUS = 5.0f;

    KeyFrame(int frame, float value, float slope) : frame(frame), value(value), slope(slope) {}

    const Vector2 getScreenPos(const Graph& graph) const;

    const Vector2 getPos() const {
        return { (float)frame, value };
    }

    bool isClicked(const Graph& graph, const Vector2& mousePos) const;

    const Vector2 getSlopeControlPos(const Graph& graph, Control control) const;

    void draw(const Graph& graph, bool isSelected) const;
    
    int frame;
    float value;
    float slope;
};
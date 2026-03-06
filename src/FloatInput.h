#pragma once

#include "utils.h"

#include <stdexcept>
#include <string>

#include "raylib.h"

struct FloatInput {
    enum class State {
        IDLE,
        CLICKED,
        DISABLED,
    };

    enum Anchor {
        cAnchor_Center = 0b0000,
        cAnchor_Top    = 0b0001,
        cAnchor_Bottom = 0b0010,
        cAnchor_Left   = 0b0100,
        cAnchor_Right  = 0b1000,
        cAnchor_TopLeft = cAnchor_Top | cAnchor_Left,
        cAnchor_TopRight = cAnchor_Top | cAnchor_Right,
        cAnchor_BottomLeft = cAnchor_Bottom | cAnchor_Left,
        cAnchor_BottomRight = cAnchor_Bottom | cAnchor_Right,
    };

    FloatInput(const Font& font, float value, Vector2 pos, Vector2 size, Anchor anchor = cAnchor_Center);

    void draw() const;
    void update(const Vector2& mousePos);

    void disable() {
        state = State::DISABLED;
    }

    void enable() {
        if (state == State::DISABLED) state = State::IDLE;
    }

    State state = State::IDLE;
    const Font& font;
    Vector2 pos = { 0, 0 };
    Vector2 size = { 0, 0 };
    Vector2 origin = { 0, 0 };
    std::string text;
    float value = 0.0f;
    float padding = 5.0f;

private:
    void saveValue() {
        if (state == State::DISABLED) return;

        state = State::IDLE;

        float tmp;
        try {
            tmp = std::stof(text);
        } catch (std::invalid_argument) {
            tmp = 2.0f;
        }
        text = format("%.1f", tmp);
        text.pop_back();
        value = std::stof(text);
    }

    bool isClicked(const Vector2& mousePos) const {
        Vector2 topLeft = { pos.x - (size.x / 2) * (1 + origin.x), pos.y - (size.y / 2) * (1 + origin.y) };
        return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, topLeft, size);
    }
};
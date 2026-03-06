#pragma once

#include "utils.h"

#include <functional>
#include <string>

#include "raylib.h"
#include "rcamera.h"

struct Button {
    enum class State {
        IDLE,
        CLICKED,
        DISABLED,
    };

    Button(const Font& font, const std::string& text, Vector2 pos, Vector2 sizeOpt);
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
    Vector2 pos;
    Vector2 size;
    std::string text;
    std::function<void()> onClick = [](){};
    std::function<void(Button&)> onUpdate = [](Button& thiz){};

private:
    void unclick() {
        if (state != State::DISABLED) state = State::IDLE;
    }

    void click() {
        state = State::CLICKED;
        onClick();
    }

    bool isClicked(const Vector2& mousePos) const {
        return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, pos, size);
    }
};
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
    };

    Button(const Font& font, const std::string& text, Vector2 pos, Vector2 sizeOpt, std::function<void()> activate) : font(font), pos(pos), text(text), onClick(activate) {
        float spacing = 1.0f;
        Vector2 textSize = MeasureTextEx(font, text.c_str(), font.baseSize, spacing);
        size.x = sizeOpt.x == 0 ? textSize.x + 10.0f : sizeOpt.x;
        size.y = sizeOpt.y == 0 ? textSize.y + 4.0f : sizeOpt.y;
    }

    void draw() const {
        Color bgColor;
        switch (state) {
        case State::IDLE:     bgColor = GRAY;      break;
        case State::CLICKED:  bgColor = DARKGRAY;  break;
        }

        DrawRectangleV(pos, size, bgColor);
        DrawTextCenter(font, text, { pos.x + size.x / 2, pos.y + size.y / 2 }, WHITE);
    }

    void update(const Vector2& mousePos) {
        if (isClicked(mousePos)) {
            click();
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            reset();
        }
    }

    State state = State::IDLE;
    const Font& font;
    Vector2 pos;
    Vector2 size;
    std::string text;
    std::function<void()> onClick;

private:
    void reset() {
        state = State::IDLE;
    }

    void click() {
        state = State::CLICKED;
        onClick();
    }

    bool isClicked(const Vector2& mousePos) const {
        return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, pos, size);
    }
};
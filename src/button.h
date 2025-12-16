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

    Button(const Font& font, const std::string& text, Vector2 pos, Vector2 sizeOpt, std::function<void()> activate) : font(font), pos(pos), text(text), onClick(activate) {
        float spacing = 1.0f;
        Vector2 textSize = MeasureTextEx(font, text.c_str(), font.baseSize, spacing);
        size.x = sizeOpt.x == 0 ? textSize.x + 20.0f : sizeOpt.x;
        size.y = sizeOpt.y == 0 ? textSize.y + 10.0f : sizeOpt.y;
    }

    void draw() const {
        // if (text == "Delete selected keyframe") printf("Button::draw: %d\n", state);

        Color bgColor, fgColor;
        switch (state) {
        case State::IDLE:     bgColor = BLUE;                       fgColor = WHITE;                   break;
        case State::CLICKED:  bgColor = DARKBLUE;                   fgColor = WHITE;                   break;
        case State::DISABLED: bgColor = ColorTint(DARKBLUE, GRAY);  fgColor = ColorTint(WHITE, GRAY);  break;
        }



        DrawRectangleV(pos, size, bgColor);
        DrawTextCenter(font, text, { pos.x + size.x / 2, pos.y + size.y / 2 }, fgColor);
    }

    void update(const Vector2& mousePos) {
        if (state == State::DISABLED) return;

        if (isClicked(mousePos)) {
            click();
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            unclick();
        }
    }

    void disable() {
        // printf("Button::disable (state: %d -> ", state);
        state = State::DISABLED;
        // printf("%d)\n", state);
    }

    void enable() {
        // printf("Button::enable (state: %d -> ", state);
        if (state == State::DISABLED) state = State::IDLE;
        // printf("%d)\n", state);
    }

    State state = State::IDLE;
    const Font& font;
    Vector2 pos;
    Vector2 size;
    std::string text;
    std::function<void()> onClick;

private:
    void unclick() {
        // printf("Button::unclick\n");
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
#include "FloatInput.h"
#include "raylib.h"

#include <string>

FloatInput::FloatInput(const Font& font, float value, Vector2 pos, Vector2 sizeOpt, Anchor anchor) : font(font), pos(pos), value(value) {
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(font, text.c_str(), font.baseSize, spacing);
    size.x = sizeOpt.x == 0 ? textSize.x + 20.0f : sizeOpt.x;
    size.y = sizeOpt.y == 0 ? textSize.y + 10.0f : sizeOpt.y;
    text = format("%.1f", value);
    text.pop_back();

    if (anchor & cAnchor_Top)    origin.y = -1;
    if (anchor & cAnchor_Bottom) origin.y =  1;
    if (anchor & cAnchor_Left)   origin.x = -1;
    if (anchor & cAnchor_Right)  origin.x =  1;
}

void FloatInput::draw() const {
    Vector2 topLeft = { pos.x - (size.x / 2) * (1 + origin.x), pos.y - (size.y / 2) * (1 + origin.y) };
    DrawRectangleV(topLeft, size, state == State::CLICKED ? GRAY : LIGHTGRAY);

    topLeft.x += padding;
    DrawTextEx(font, text.c_str(), topLeft, font.baseSize, 1.0f, BLACK);
}

void FloatInput::update(const Vector2& mousePos) {
    size = MeasureTextEx(font, text.c_str(), font.baseSize, 1.0f);
    size.x += padding * 2;

    Vector2 topLeft = { pos.x - (size.x / 2) * (1 + origin.x), pos.y - (size.y / 2) * (1 + origin.y) };

    if (state == State::IDLE) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, topLeft, size)) state = State::CLICKED;
    } else if (state == State::CLICKED) {
        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }
        
        if (text.length() <= 6) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= KEY_ZERO && key <= KEY_NINE) text.push_back((char)key);

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_PERIOD) && text.find('.') == std::string::npos) {
                text.push_back('.');
            }
        }


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsPointInRect(mousePos, topLeft, size)) saveValue();
        else if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) saveValue();
    }
}

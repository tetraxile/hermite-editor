#include "button.h"

Button::Button(const Font& font, const std::string& text, Vector2 pos, Vector2 sizeOpt) : font(font), pos(pos), text(text) {
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(font, text.c_str(), font.baseSize, spacing);
    size.x = sizeOpt.x == 0 ? textSize.x + 20.0f : sizeOpt.x;
    size.y = sizeOpt.y == 0 ? textSize.y + 10.0f : sizeOpt.y;
}

void Button::draw() const {
    Color bgColor, fgColor;
    switch (state) {
    case State::IDLE:     bgColor = BLUE;                       fgColor = WHITE;                   break;
    case State::CLICKED:  bgColor = DARKBLUE;                   fgColor = WHITE;                   break;
    case State::DISABLED: bgColor = ColorTint(DARKBLUE, GRAY);  fgColor = ColorTint(WHITE, GRAY);  break;
    }

    DrawRectangleV(pos, size, bgColor);
    DrawTextCenter(font, text, { pos.x + size.x / 2, pos.y + size.y / 2 }, fgColor);
}

void Button::update(const Vector2& mousePos) {
    onUpdate(*this);

    if (state == State::DISABLED) return;

    if (isClicked(mousePos)) {
        click();
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        unclick();
    }
}
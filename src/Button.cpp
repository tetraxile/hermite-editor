#include "Button.h"
#include "raylib.h"

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

    DrawRectangleRounded({ pos.x, pos.y, size.x, size.y }, 1.0f, 10, bgColor);
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
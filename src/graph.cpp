#include "graph.h"
#include "keyframe.h"
#include "utils.h"

#include "raymath.h"

float HermiteInterpolate(float y_0, float y_1, float m_0, float m_1, float diff, float weight) {
    float result;

    result = y_0 + (y_0 - y_1) * (2 * weight - 3) * weight * weight;
    result += (diff * (weight - 1)) * (m_0 * (weight - 1) + m_1 * weight);

    return result;
}

// credit to https://github.com/gdkchan/SPICA/blob/42c4181e198b0fd34f0a567345ee7e75b54cb58b/SPICA/Formats/CtrH3D/Animation/H3DFloatKeyFrameGroup.cs
float GetInterpolatedValue(const std::vector<KeyFrame>& keyframes, float frame) {
    if (keyframes.size() == 0) return 0;
    if (keyframes.size() == 1) return keyframes[0].value;

    const KeyFrame* leftKeyframe = &keyframes.front();
    const KeyFrame* rightKeyframe = &keyframes.back();

    for (const KeyFrame& keyframe : keyframes) {
        if (keyframe.frame <= frame)
            leftKeyframe = &keyframe;
        if (keyframe.frame >= frame && keyframe.frame < rightKeyframe->frame)
            rightKeyframe = &keyframe;
    }

    if (leftKeyframe->frame == rightKeyframe->frame) return leftKeyframe->value;

    float frameDiff = frame - leftKeyframe->frame;
    float weight = frameDiff / (rightKeyframe->frame - leftKeyframe->frame);

    return HermiteInterpolate(leftKeyframe->value, rightKeyframe->value, leftKeyframe->slope, rightKeyframe->slope, frameDiff, weight);
}

Graph::Graph(const Font& font, int screenWidth, int screenHeight) : font(font), bottom(screenHeight - 200.0f), right(screenWidth - 100.0f) {        
    keyframes.push_back({ 0,  1.00f, -0.44f });
    keyframes.push_back({ 4,  0.53f, -0.03f });
    keyframes.push_back({ 15, 0.50f,  0.00f });
    keyframes.push_back({ 26, 0.53f,  0.03f });
    keyframes.push_back({ 30, 1.00f,  0.44f });
}

const Vector2 Graph::coordToScreenPos(const Vector2& coord) const {
    float xPos = left + coord.x * frameWidth;
    float yPos = bottom - coord.y * height / yRange;
    return { xPos, yPos };
}

const Vector2 Graph::screenPosToCoord(const Vector2& screenPos) const {
    float xCoord = (screenPos.x - left) / frameWidth;
    float yCoord = (bottom - screenPos.y) / height * yRange;
    return { xCoord, yCoord };
}

void Graph::resize(int screenWidth, int screenHeight) {
    bottom = screenHeight - 200.0f;
    right = screenWidth - 100.0f;
    width = right - left;
    height = bottom - top;
    yTickHeight = height / yTickCount;
    size = { width, height };
    frameWidth = width / frameCount;
}

void Graph::draw() const {
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

    // draw hermite interpolation
    if (keyframes.empty()) return;
    const float step = 1.0f;
    for (float xPos = left + step; xPos < right + step / 2 && xPos < keyframes.back().getScreenPos(*this).x; xPos += step) {
        float interpFrame = screenPosToCoord({ xPos, 0.0f }).x;
        float interpValue = GetInterpolatedValue(keyframes, interpFrame);
        
        float yPos = coordToScreenPos({ 0.0f, interpValue }).y;

        DrawCircleV({ xPos, yPos }, 2.0f, ColorAlpha(GREEN, 0.5f));
    }

    drawKeyframes();
}

void Graph::drawKeyframes() const {
    // draw ghost keyframe point
    if (isClickingNewKeyframe) {
        DrawCircleV(ghostKeyframePos, KeyFrame::RADIUS, ColorAlpha(RED, 0.5f));
    }

    // draw keyframe points
    for (const KeyFrame& keyframe : keyframes) {
        keyframe.draw(*this, selectedKeyframe == &keyframe);
    }

    // draw selected keyframe's data
    {
        Vector2 textTopCenter = { left + width / 2, bottom + 50.0f };
        Vector2 padding = { 10.0f, 5.0f };

        std::string text;
        if (selectedKeyframe != nullptr)
            text = format("Selected keyframe:\nFrame: %d\nValue: %.03f\nSlope: %.03f", selectedKeyframe->frame, selectedKeyframe->value, selectedKeyframe->slope);
        else
            text = "Selected keyframe:";

        Vector2 textSize = MeasureTextEx(font, text.c_str(), font.baseSize, 1.0f) + padding * 2;
        DrawRectangleV(textTopCenter - Vector2 { textSize.x / 2, 0.0f }, textSize, GRAY);
        DrawTextTopCenter(font, text, textTopCenter + Vector2 { 0.0f, padding.y }, WHITE);
    } 
}

void Graph::update(const Vector2& mousePos) {
    // update which keyframe is selected
    if (!isClickingNewKeyframe) {
        for (KeyFrame& keyframe : keyframes) {
            if (keyframe.isClicked(*this, mousePos)) {
                selectedKeyframe = &keyframe;
                draggingKeyframe = nullptr;
                selectedKeyframeControl = KeyFrame::Control::NONE;
                keyframePixelsMoved = 0.0f;
                break;
            }
        }
    }

    // deselect keyframe if it goes off screen
    if (selectedKeyframe != nullptr && selectedKeyframe->frame > frameCount) {
        selectedKeyframe = nullptr;
    }

    // control selected keyframe
    if (selectedKeyframe != nullptr) {
        // update which control is being moved

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, topLeft, size)) {
            draggingKeyframe = selectedKeyframe;
            keyframePixelsMoved = 0.0f;
        }

        Vector2 keyframePos = selectedKeyframe->getScreenPos(*this);
        Vector2 leftControlPos = selectedKeyframe->getSlopeControlPos(*this, KeyFrame::Control::LEFT);
        Vector2 rightControlPos = selectedKeyframe->getSlopeControlPos(*this, KeyFrame::Control::RIGHT);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInCircle(mousePos, leftControlPos, KeyFrame::RADIUS * 2)) {
            selectedKeyframeControl = KeyFrame::Control::LEFT;
            draggingKeyframe = nullptr;
            keyframePixelsMoved = 0.0f;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInCircle(mousePos, rightControlPos, KeyFrame::RADIUS * 2)) {
            selectedKeyframeControl = KeyFrame::Control::RIGHT;
            draggingKeyframe = nullptr;
            keyframePixelsMoved = 0.0f;
        }

        // move the relevant control

        if (draggingKeyframe != nullptr) {
            if (keyframePixelsMoved < 10)
                keyframePixelsMoved = abs(draggingKeyframe->getScreenPos(*this).y - mousePos.y);

            if (keyframePixelsMoved >= 10) {
                if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    int tickIdx = (mousePos.y - top) / height * yTickCount + 0.5f;
                    draggingKeyframe->value = screenPosToCoord({ 0, top + tickIdx * yTickHeight }).y;
                } else {
                    draggingKeyframe->value = screenPosToCoord(mousePos).y;
                }
            }
        }

        if (selectedKeyframeControl != KeyFrame::Control::NONE) {
            Vector2 slopeDir = mousePos - keyframePos;
            
            float run = (slopeDir.x / frameWidth);
            float rise = -(slopeDir.y / height * yRange);
            float slope = rise / run;

            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                slope = (float)((int)(slope * 10 + 0.5f) / 10.0f);
            }

            selectedKeyframe->slope = slope;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        selectedKeyframeControl = KeyFrame::Control::NONE;
        draggingKeyframe = nullptr;
        keyframePixelsMoved = 0.0f;
    }

    // update ghost keyframe position
    if (IsPointInRect(mousePos, topLeft, size) && isClickingNewKeyframe) {
        int ghostKeyframeFrame = (mousePos.x - left) / width * frameCount + 0.5f;
        ghostKeyframePos.x = left + ghostKeyframeFrame * frameWidth;

        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            int ghostKeyframeTick = (mousePos.y - top) / height * yTickCount + 0.5f;
            ghostKeyframePos.y = top + ghostKeyframeTick * yTickHeight;
        } else {
            ghostKeyframePos.y = mousePos.y;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float ghostKeyframeValue = screenPosToCoord(ghostKeyframePos).y;
            KeyFrame newKeyframe = { ghostKeyframeFrame, ghostKeyframeValue, 0.0f };

            int leftIndex = 0;
            int rightIndex = keyframes.size();
            for (int i = 0; i < keyframes.size(); i++) {
                const KeyFrame& keyframe = keyframes[i];

                if (keyframe.frame == newKeyframe.frame) {
                    leftIndex = -1;
                    rightIndex = keyframes.size();
                    break;
                }

                if (keyframe.frame <= newKeyframe.frame)
                    leftIndex = i;
                if (keyframe.frame >= newKeyframe.frame && i < rightIndex)
                    rightIndex = i;
            }

            int newKeyframeIndex = -1;
            if (leftIndex != -1 || rightIndex != keyframes.size())
                newKeyframeIndex = rightIndex;

            if (newKeyframeIndex != -1) {
                keyframes.insert(keyframes.begin() + newKeyframeIndex, newKeyframe);
                selectedKeyframe = &keyframes[newKeyframeIndex];
                isClickingNewKeyframe = false;
            }
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        isClickingNewKeyframe = false;
        selectedKeyframe = nullptr;
    }
}
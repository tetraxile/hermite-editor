#include "graph.h"

#include "keyframe.h"

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

void Graph::draw(const std::vector<KeyFrame>& keyframes) const {
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
    float prevValue = GetInterpolatedValue(keyframes, 0.0f);
    const float step = 1.0f;
    for (float xPos = left + step; xPos < right + step / 2 && xPos < keyframes.back().getScreenPos(*this).x; xPos += step) {
        float interpFrame = screenPosToCoord({ xPos, 0.0f }).x;
        float interpValue = GetInterpolatedValue(keyframes, interpFrame);
        
        float yPos = coordToScreenPos({ 0.0f, interpValue }).y;
        float prevYPos = coordToScreenPos({ 0.0f, prevValue }).y;
        DrawLineEx({ xPos - step, prevYPos }, { xPos, yPos }, 2, ColorAlpha(BLUE, 0.5f));

        prevValue = interpValue;
    }
}
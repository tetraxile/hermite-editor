#include "raylib.h"
#include "rcamera.h"

#include "graph.h"
#include "button.h"
#include "keyframe.h"
#include "utils.h"
#include <cstdlib>


float HermiteInterpolate(float y_0, float y_1, float m_0, float m_1, float diff, float weight)
{
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

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "hermite animation editor");

    SetTargetFPS(60);

    const Font font = LoadFontEx("../assets/SourceCodePro-Regular.ttf", 25, nullptr, 0);

    Graph graph(font);
    
    std::vector<KeyFrame> keyframes;
    KeyFrame* selectedKeyframe = nullptr;
    KeyFrame* draggingKeyframe = nullptr;
    float keyframePixelsMoved = 0.0f;
    KeyFrame::Control selectedKeyframeControl = KeyFrame::Control::NONE;
    
    keyframes.push_back({ 0,  1.00f, -0.44f });
    keyframes.push_back({ 4,  0.53f, -0.03f });
    keyframes.push_back({ 15, 0.50f,  0.00f });
    keyframes.push_back({ 26, 0.53f,  0.03f });
    keyframes.push_back({ 30, 1.00f,  0.44f });

    Vector2 ghostKeyframePos;
    bool isClickingNewKeyframe = false;

    std::vector<Button> buttons;

    // timeline frame count buttons
    buttons.push_back({ font, "+", { graph.right + 30, graph.bottom - 20 }, { 20, 20 },
        [&graph](){
            graph.frameCount++;
            graph.frameWidth = graph.width / graph.frameCount;
        }
    });
    buttons.push_back({ font, "-", { graph.right + 30, graph.bottom + 20 }, { 20, 20 },
        [&graph](){
            if (graph.frameCount > 0) {
                graph.frameCount--;
                graph.frameWidth = graph.width / graph.frameCount;
            }
        }
    });

    // "add keyframe" button
    buttons.push_back({ font, "Add keyframe", { 100, 30 }, { 0, 0 },
        [&isClickingNewKeyframe, &selectedKeyframe](){
            isClickingNewKeyframe = true;
            selectedKeyframe = nullptr;
        }
    });

    // "delete keyframe" button
    buttons.push_back({ font, "Delete keyframe", { buttons.back().pos.x + buttons.back().size.x + 20, 30 }, { 0, 0 },
        [&keyframes, &selectedKeyframe, &isClickingNewKeyframe](){
            isClickingNewKeyframe = false;
            for (int i = 0; i < keyframes.size(); i++) {
                KeyFrame& keyframe = keyframes[i];
                if (&keyframe == selectedKeyframe) {
                    selectedKeyframe = nullptr;
                    keyframes.erase(keyframes.begin() + i);
                    break;
                }
            }
        }
    });

    // "copy to clipboard" button
    buttons.push_back({ font, "Copy to clipboard", { buttons.back().pos.x + buttons.back().size.x + 20, 30 }, { 0, 0 },
        [keyframes](){
            std::string output = "a";
            for (const KeyFrame& keyframe : keyframes) {
                std::string addition = format("<KeyFrame Frame=\"%d\" Value=\"%f\" Slope=\"%f\"/>\n", keyframe.frame, keyframe.value, keyframe.slope);
                output.pop_back();
                output.append(addition);
            }
            SetClipboardText(output.c_str());
        }
    });
    
    
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------

        Vector2 mousePos = GetMousePosition();

        // update buttons
        for (Button& button : buttons) {
            button.update(mousePos);
        }

        // update which keyframe is selected
        if (!isClickingNewKeyframe) {
            for (KeyFrame& keyframe : keyframes) {
                if (keyframe.isClicked(graph, mousePos)) {
                    selectedKeyframe = &keyframe;
                    draggingKeyframe = nullptr;
                    selectedKeyframeControl = KeyFrame::Control::NONE;
                    keyframePixelsMoved = 0.0f;
                    break;
                }
            }
        }

        // deselect keyframe if it goes off screen
        if (selectedKeyframe != nullptr && selectedKeyframe->frame > graph.frameCount) selectedKeyframe = nullptr;

        // control selected keyframe
        if (selectedKeyframe != nullptr) {
            // update which control is being moved

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInRect(mousePos, graph.topLeft, graph.size)) {
                draggingKeyframe = selectedKeyframe;
                keyframePixelsMoved = 0.0f;
            }

            Vector2 keyframePos = selectedKeyframe->getScreenPos(graph);
            Vector2 leftControlPos = selectedKeyframe->getSlopeControlPos(graph, KeyFrame::Control::LEFT);
            Vector2 rightControlPos = selectedKeyframe->getSlopeControlPos(graph, KeyFrame::Control::RIGHT);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInCircle(mousePos, leftControlPos, KeyFrame::RADIUS)) {
                selectedKeyframeControl = KeyFrame::Control::LEFT;
                draggingKeyframe = nullptr;
                keyframePixelsMoved = 0.0f;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsPointInCircle(mousePos, rightControlPos, KeyFrame::RADIUS)) {
                selectedKeyframeControl = KeyFrame::Control::RIGHT;
                draggingKeyframe = nullptr;
                keyframePixelsMoved = 0.0f;
            }

            // move the relevant control

            if (draggingKeyframe != nullptr) {
                if (keyframePixelsMoved < KeyFrame::RADIUS * 2)
                    keyframePixelsMoved = abs(draggingKeyframe->getScreenPos(graph).y - mousePos.y);

                if (keyframePixelsMoved >= KeyFrame::RADIUS * 2) {
                    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                        int tickIdx = (mousePos.y - graph.top) / graph.height * graph.yTickCount + 0.5f;
                        draggingKeyframe->value = graph.screenPosToCoord({ 0, graph.top + tickIdx * graph.yTickHeight }).y;
                    } else {
                        draggingKeyframe->value = graph.screenPosToCoord(mousePos).y;
                    }
                }
            }

            if (selectedKeyframeControl != KeyFrame::Control::NONE) {
                Vector2 slopeDir = mousePos - keyframePos;
                
                float run = (slopeDir.x / graph.frameWidth);
                float rise = -(slopeDir.y / graph.height * graph.yRange);
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
        if (IsPointInRect(mousePos, graph.topLeft, graph.size) && isClickingNewKeyframe) {
            int ghostKeyframeFrame = (mousePos.x - graph.left) / graph.width * graph.frameCount + 0.5f;
            ghostKeyframePos.x = graph.left + ghostKeyframeFrame * graph.frameWidth;

            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                int ghostKeyframeTick = (mousePos.y - graph.top) / graph.height * graph.yTickCount + 0.5f;
                ghostKeyframePos.y = graph.top + ghostKeyframeTick * graph.yTickHeight;
            } else {
                ghostKeyframePos.y = mousePos.y;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float ghostKeyframeValue = graph.screenPosToCoord(ghostKeyframePos).y;
                KeyFrame newKeyframe = { ghostKeyframeFrame, ghostKeyframeValue, 0.0f };

                int newKeyframeIndex = 0;
                for (int i = 0; i < keyframes.size(); i++) {
                    const KeyFrame& keyframe = keyframes[i];
                    if (keyframe.frame == newKeyframe.frame) {
                        newKeyframeIndex = -1;
                        break;
                    }
                    if (keyframe.frame > newKeyframe.frame) {
                        newKeyframeIndex = i;
                        break;
                    }
                }
                if (newKeyframeIndex != -1) {
                    keyframes.insert(keyframes.begin() + newKeyframeIndex, newKeyframe);
                    selectedKeyframe = &keyframes[newKeyframeIndex];
                    isClickingNewKeyframe = false;
                }
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                isClickingNewKeyframe = false;
            }
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            graph.draw();

            // draw hermite interpolation
            float prevValue = GetInterpolatedValue(keyframes, 0.0f);
            const float step = 1.0f;
            for (float xPos = graph.left + step; xPos < graph.right + step / 2 && xPos < keyframes.back().getScreenPos(graph).x; xPos += step) {
                float interpFrame = graph.screenPosToCoord({ xPos, 0.0f }).x;
                float interpValue = GetInterpolatedValue(keyframes, interpFrame);
                
                float yPos = graph.coordToScreenPos({ 0.0f, interpValue }).y;
                float prevYPos = graph.coordToScreenPos({ 0.0f, prevValue }).y;
                DrawLineEx({ xPos - step, prevYPos }, { xPos, yPos }, 2, ColorAlpha(BLUE, 0.5f));

                prevValue = interpValue;
            }

            // draw ghost keyframe point
            if (isClickingNewKeyframe) {
                DrawCircleV(ghostKeyframePos, KeyFrame::RADIUS, ColorAlpha(RED, 0.5f));
            }

            // draw keyframe points
            for (const KeyFrame& keyframe : keyframes) {
                keyframe.draw(graph, selectedKeyframe == &keyframe);
            }
            
            // draw buttons
            for (const Button& button : buttons) {
                button.draw();
            }


        EndDrawing();
    }

    CloseWindow();

    return 0;
}

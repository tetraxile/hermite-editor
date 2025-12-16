#include "graph.h"
#include "button.h"
#include "keyframe.h"
#include "utils.h"

#include <cstdlib>

#include "raylib.h"
#include "rcamera.h"

int main() {
    int screenWidth = 1600;
    int screenHeight = 900;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetWindowMinSize(500, 500);
    InitWindow(screenWidth, screenHeight, "hermite animation editor");

    SetTargetFPS(60);

    const Font font = LoadFontEx("../assets/SourceCodePro-Regular.ttf", 25, nullptr, 0);

    Graph graph(font, screenWidth, screenHeight);
    
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

    Button frameCountUpButton = { font, "+", { graph.right + 30, graph.bottom - 20 }, { 20, 20 } };
    Button frameCountDownButton = { font, "-", { graph.right + 30, graph.bottom + 20 }, { 20, 20 } };

    Button addButton = { font, "Add keyframe", { 100, 30 }, { 0, 0 } };
    Button deleteButton = { font, "Delete selected keyframe", { addButton.pos.x + addButton.size.x + 20, 30 }, { 0, 0 } };
    Button clearButton = { font, "Clear all keyframes", { deleteButton.pos.x + deleteButton.size.x + 20, 30 }, { 0, 0 } };
    Button copyButton = { font, "Copy to clipboard", { clearButton.pos.x + clearButton.size.x + 20, 30 }, { 0, 0 } };
    
    frameCountUpButton.onClick = [&graph](){
        graph.frameCount++;
        graph.frameWidth = graph.width / graph.frameCount;
    };

    frameCountUpButton.onUpdate = [&graph](Button& thiz){
        thiz.pos = { graph.right + 30, graph.bottom - 20 };
    };

    frameCountDownButton.onClick = [&graph](){
        if (graph.frameCount > 0) {
            graph.frameCount--;
            graph.frameWidth = graph.width / graph.frameCount;
        }
    };

    frameCountDownButton.onUpdate = [&graph](Button& thiz){
        thiz.pos = { graph.right + 30, graph.bottom + 20 };
    };

    addButton.onClick = [&isClickingNewKeyframe, &selectedKeyframe, deleteButton](){
        isClickingNewKeyframe = true;
        selectedKeyframe = nullptr;
    };

    deleteButton.onClick = [&keyframes, &selectedKeyframe, &isClickingNewKeyframe](){
        isClickingNewKeyframe = false;
        for (int i = 0; i < keyframes.size(); i++) {
            KeyFrame& keyframe = keyframes[i];
            if (&keyframe == selectedKeyframe) {
                selectedKeyframe = nullptr;
                keyframes.erase(keyframes.begin() + i);
                break;
            }
        }
    };

    clearButton.onClick = [&keyframes, &selectedKeyframe, &isClickingNewKeyframe, deleteButton](){
        isClickingNewKeyframe = false;
        selectedKeyframe = nullptr;
        keyframes.clear();
    };

    copyButton.onClick = [keyframes](){
        if (keyframes.empty()) return;
        std::string output = "a";
        for (const KeyFrame& keyframe : keyframes) {
            std::string addition = format("<KeyFrame Frame=\"%d\" Value=\"%f\" Slope=\"%f\"/>\n", keyframe.frame, keyframe.value, keyframe.slope);
            output.pop_back();
            output.append(addition);
        }
        SetClipboardText(output.c_str());
    };
    
    buttons.push_back(addButton);
    buttons.push_back(deleteButton);
    buttons.push_back(clearButton);
    buttons.push_back(copyButton);
    buttons.push_back(frameCountUpButton);
    buttons.push_back(frameCountDownButton);
    
    
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------

        if (IsWindowResized()) {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();

            graph.resize(screenWidth, screenHeight);
        }

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
        if (selectedKeyframe != nullptr && selectedKeyframe->frame > graph.frameCount) {
            selectedKeyframe = nullptr;
        }

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
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            isClickingNewKeyframe = false;
            selectedKeyframe = nullptr;
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            graph.draw(keyframes);

            // draw ghost keyframe point
            if (isClickingNewKeyframe) {
                DrawCircleV(ghostKeyframePos, KeyFrame::RADIUS, ColorAlpha(RED, 0.5f));
            }

            // draw keyframe points
            for (const KeyFrame& keyframe : keyframes) {
                keyframe.draw(graph, selectedKeyframe == &keyframe);
            }

            // draw selected keyframe's data
            {
                Vector2 textTopCenter = { graph.left + graph.width / 2, graph.bottom + 50.0f };
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

            // draw buttons
            for (Button& button : buttons) {
                button.draw();
            }


        EndDrawing();
    }

    CloseWindow();

    return 0;
}

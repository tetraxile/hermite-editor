#include "graph.h"
#include "button.h"
#include "keyframe.h"
#include "utils.h"

#include "raylib.h"
#include "rcamera.h"

void initButtons(Graph& graph, std::vector<Button>& buttons) {
    Button frameCountUpButton =   { graph.font, "+", { graph.right + 30, graph.bottom - 20 }, { 20, 20 } };
    Button frameCountDownButton = { graph.font, "-", { graph.right + 30, graph.bottom + 20 }, { 20, 20 } };

    Button addButton =    { graph.font, "Add keyframe",             { 100, 30 },                                           { 0, 0 } };
    Button deleteButton = { graph.font, "Delete selected keyframe", {    addButton.pos.x +    addButton.size.x + 20, 30 }, { 0, 0 } };
    Button clearButton =  { graph.font, "Clear all keyframes",      { deleteButton.pos.x + deleteButton.size.x + 20, 30 }, { 0, 0 } };
    Button copyButton =   { graph.font, "Copy to clipboard",        {  clearButton.pos.x +  clearButton.size.x + 20, 30 }, { 0, 0 } };
    
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

    addButton.onClick = [&graph](){
        graph.isClickingNewKeyframe = true;
        graph.selectedKeyframe = nullptr;
    };

    deleteButton.onClick = [&graph](){
        graph.isClickingNewKeyframe = false;
        for (int i = 0; i < graph.keyframes.size(); i++) {
            KeyFrame& keyframe = graph.keyframes[i];
            if (&keyframe == graph.selectedKeyframe) {
                graph.selectedKeyframe = nullptr;
                graph.keyframes.erase(graph.keyframes.begin() + i);
                break;
            }
        }
    };

    clearButton.onClick = [&graph](){
        graph.isClickingNewKeyframe = false;
        graph.selectedKeyframe = nullptr;
        graph.keyframes.clear();
    };

    copyButton.onClick = [&graph](){
        if (graph.keyframes.empty()) return;
        std::string output = "a";
        for (const KeyFrame& keyframe : graph.keyframes) {
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
}

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

    std::vector<Button> buttons;
    initButtons(graph, buttons);
    
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

        graph.update(mousePos);


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            graph.draw();

            // draw buttons
            for (Button& button : buttons) {
                button.draw();
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

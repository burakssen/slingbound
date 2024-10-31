#include "MainScene.h"
#include <iostream>
#include <cmath>
#include <raymath.h>

void MainScene::Initialize(entt::registry &registry)
{
    this->m_animations.clear();

    this->m_animations.emplace_back(
        Vector2{GetScreenWidth() / 4.0f, GetScreenHeight() / 2.0f},
        0.5f,   // Damping
        200.0f, // Length increased
        2.0f    // Angle range
    );

    this->m_animations.emplace_back(
        Vector2{3 * GetScreenWidth() / 4.0f, GetScreenHeight() / 2.0f},
        0.8f,   // Less damping
        100.0f, // Shorter length
        5.0f    // Wider angle range
    );
}

void MainScene::HandleInput(entt::registry &registry)
{
}

void MainScene::Update(entt::registry &registry)
{
    float deltaTime = GetFrameTime();

    // Update pendulum animations
    for (auto &animation : m_animations)
    {
        animation.UpdatePendulum(deltaTime);
    }
}

void MainScene::Draw(entt::registry &registry)
{

    // Subtle star-like background
    for (int i = 0; i < 100; i++)
    {
        int x = GetRandomValue(0, GetScreenWidth());
        int y = GetRandomValue(0, GetScreenHeight());
        float size = GetRandomValue(1, 3);
        DrawCircle(x, y, size, Fade(WHITE, 0.3f * sinf(GetTime() * 2 + i)));
    }

    // Draw game title with dynamic effect
    const char *title = "SLINGBOUND";
    int fontSize = 60;
    int titleWidth = MeasureText(title, fontSize);

    // Title with wave effect
    for (int i = 0; i < strlen(title); i++)
    {
        char letter[2] = {title[i], '\0'};
        float wave = sinf(GetTime() * 3 + i * 0.5f) * 10;
        DrawText(
            letter,
            GetScreenWidth() / 2.0f - titleWidth / 2.0f + i * (fontSize * 0.6f),
            GetScreenHeight() / 2.0f - 200.0f + wave,
            fontSize,
            Fade(WHITE, 0.8f + 0.2f * sinf(GetTime() * 2 + i)));
    }

    // Button configurations
    const float BUTTON_WIDTH = 250.0f;
    const float BUTTON_HEIGHT = 70.0f;
    const float BUTTON_PADDING = 20.0f;

    // Hover and click detection variables
    Vector2 mousePoint = GetMousePosition();

    // Level Selection Button
    Rectangle levelButton = {
        GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
        GetScreenHeight() / 2.0f - BUTTON_HEIGHT / 2.0f,
        BUTTON_WIDTH,
        BUTTON_HEIGHT};

    bool levelHover = CheckCollisionPointRec(mousePoint, levelButton);
    Color levelButtonColor = levelHover ? BLUE : GRAY;

    // Draw button with hover effect
    DrawRectangleRounded(
        Rectangle{
            levelButton.x - (levelHover ? 2 : 0),
            levelButton.y - (levelHover ? 2 : 0),
            levelButton.width + (levelHover ? 4 : 0),
            levelButton.height + (levelHover ? 4 : 0)},
        0.2f, 10,
        Fade(BLACK, 0.3f) // Shadow
    );

    DrawRectangleRounded(levelButton, 0.2f, 10, levelButtonColor);
    DrawText(
        "Level Selection",
        levelButton.x + levelButton.width / 2 - MeasureText("Level Selection", 20) / 2,
        levelButton.y + levelButton.height / 2 - 10,
        20,
        WHITE);

    // Handle button click
    if (levelHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        this->m_sceneManager->ChangeScene(SceneManager::SceneType::LevelSelectionScene);
    }

    // Second button (different based on platform)
    Rectangle secondButton = {
        GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
        GetScreenHeight() / 2.0f + BUTTON_HEIGHT + BUTTON_PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT};

    bool secondHover = CheckCollisionPointRec(mousePoint, secondButton);
    Color secondButtonColor = secondHover ? GREEN : GRAY;

    // Draw second button with hover effect
    DrawRectangleRounded(
        Rectangle{
            secondButton.x - (secondHover ? 2 : 0),
            secondButton.y - (secondHover ? 2 : 0),
            secondButton.width + (secondHover ? 4 : 0),
            secondButton.height + (secondHover ? 4 : 0)},
        0.2f, 10,
        Fade(BLACK, 0.3f) // Shadow
    );

    DrawRectangleRounded(secondButton, 0.2f, 10, secondButtonColor);

#ifdef __EMSCRIPTEN__
    DrawText(
        "About",
        secondButton.x + secondButton.width / 2 - MeasureText("About", 20) / 2,
        secondButton.y + secondButton.height / 2 - 10,
        20,
        WHITE);

    if (secondHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // If no GameOverScene, you might want to just exit the application
        this->m_sceneManager->ChangeScene(SceneManager::SceneType::AboutScene);
    }
#else
    DrawText(
        "Quit Game",
        secondButton.x + secondButton.width / 2 - MeasureText("Quit", 20) / 2,
        secondButton.y + secondButton.height / 2 - 10,
        20,
        WHITE);

    if (secondHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
// Simple quit option if no other scene is available
#ifndef __EMSCRIPTEN__
        CloseWindow(); // Raylib function to close the window
        exit(0);
#endif
    }
#endif

    // Draw pendulum animations
    for (size_t i = 0; i < this->m_animations.size(); i++)
    {
        // More stylized pendulum rendering
        DrawLineEx(
            this->m_animations[i].anchorPosition,
            this->m_animations[i].playerPosition,
            5.0f, // Thicker line
            Fade(GREEN, 0.7f));

        DrawCircleV(this->m_animations[i].anchorPosition, ANCHOR_RADIUS * 1.2f, BLUE);
        DrawCircleV(this->m_animations[i].playerPosition, PLAYER_RADIUS * 1.2f, RED);
    }
}

void MainScene::Shutdown(entt::registry &registry)
{
    // Clear animations
    this->m_animations.clear();
}
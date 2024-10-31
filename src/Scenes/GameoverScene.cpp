#include "GameoverScene.h"
#include <iostream>
#include <raylib.h>

void GameoverScene::Initialize(entt::registry &registry)
{
}

void GameoverScene::HandleInput(entt::registry &registry)
{
}

void GameoverScene::Update(entt::registry &registry)
{
}

void GameoverScene::Draw(entt::registry &registry)
{
    DrawText("Game Over",
             GetScreenWidth() / 2 - MeasureText("Game Over", 60) / 2,
             GetScreenHeight() / 2 - 300,
             60,
             RED);

    auto levelView = registry.view<Level>();
    if (levelView.begin() != levelView.end())
    {
        auto levelEntity = levelView.front();
        auto &levelComponent = registry.get<Level>(levelEntity);
        DrawText(levelComponent.message.c_str(),
                 GetScreenWidth() / 2 - MeasureText(levelComponent.message.c_str(), 40) / 2,
                 GetScreenHeight() / 2 - 200,
                 40,
                 GRAY);
    }

    // Button configurations
    const float BUTTON_WIDTH = 200.0f;
    const float BUTTON_HEIGHT = 80.0f;
    const float BUTTON_PADDING = 20.0f;
    Vector2 mousePoint = GetMousePosition();

    // Restart Button
    Rectangle restartButton = {
        GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
        GetScreenHeight() / 2.0f,
        BUTTON_WIDTH,
        BUTTON_HEIGHT};

    bool restartHover = CheckCollisionPointRec(mousePoint, restartButton);
    Color restartButtonColor = restartHover ? BLUE : GRAY;

    DrawRectangleRounded(restartButton, 0.2f, 10, restartButtonColor);
    DrawText(
        "Restart",
        restartButton.x + restartButton.width / 2 - MeasureText("Restart", 20) / 2,
        restartButton.y + restartButton.height / 2 - 10,
        20,
        WHITE);

    if (restartHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        auto view = registry.view<Level>();
        for (auto entity : view)
        {
            auto &level = view.get<Level>(entity);
            this->m_sceneManager->ChangeScene(static_cast<SceneManager::SceneType>(level.currentLevel));
            break;
        }
    }

    // Level Selection Button
    Rectangle levelSelectionButton = {
        GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
        GetScreenHeight() / 2.0f + BUTTON_HEIGHT + BUTTON_PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT};

    bool levelSelectionHover = CheckCollisionPointRec(mousePoint, levelSelectionButton);
    Color levelSelectionButtonColor = levelSelectionHover ? GREEN : GRAY;

    DrawRectangleRounded(levelSelectionButton, 0.2f, 10, levelSelectionButtonColor);
    DrawText(
        "Level Selection",
        levelSelectionButton.x + levelSelectionButton.width / 2 - MeasureText("Level Selection", 20) / 2,
        levelSelectionButton.y + levelSelectionButton.height / 2 - 10,
        20,
        WHITE);

    if (levelSelectionHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        this->m_sceneManager->ChangeScene(SceneManager::SceneType::LevelSelectionScene);
    }
}

void GameoverScene::Shutdown(entt::registry &registry)
{
}

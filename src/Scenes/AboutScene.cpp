#include "AboutScene.h"

void AboutScene::Initialize(entt::registry &registry)
{
}

void AboutScene::HandleInput(entt::registry &registry)
{
    // Handle input specific to GameoverScene
}

void AboutScene::Update(entt::registry &registry)
{
}

void AboutScene::Draw(entt::registry &registry)
{
    // Draw specific to GameoverScene
    DrawText(info.c_str(), GetScreenWidth() / 2 - MeasureText(info.c_str(), 40) / 2, GetScreenHeight() / 2 - 220, 40, GRAY);
    DrawText(by.c_str(), GetScreenWidth() / 2 - MeasureText(by.c_str(), 40) / 2, GetScreenHeight() / 2 - 120, 40, GRAY);

    DrawText("Used colors", GetScreenWidth() / 2 - MeasureText("Used colors", 40) / 2, GetScreenHeight() / 2 - 20, 40, GRAY);
    for (int i = 0; i < colors.size(); i++)
    {
        DrawRectangle(GetScreenWidth() / 2 - 100 + i * 25, GetScreenHeight() / 2 + 20, 20, 20, colors[i]);
        DrawRectangleLines(GetScreenWidth() / 2 - 100 + i * 25, GetScreenHeight() / 2 + 20, 20, 20, WHITE);
    }

    // Button configurations
    const float BUTTON_WIDTH = 200.0f;
    const float BUTTON_HEIGHT = 80.0f;
    const float BUTTON_PADDING = 20.0f;
    Vector2 mousePoint = GetMousePosition();

    // Back Button
    Rectangle backButton = {
        GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
        GetScreenHeight() / 2.0f + BUTTON_HEIGHT + BUTTON_PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT};

    bool backHover = CheckCollisionPointRec(mousePoint, backButton);
    Color backButtonColor = backHover ? BLUE : GRAY;

    DrawRectangleRounded(backButton, 0.2f, 10, backButtonColor);
    DrawText("Go Back", backButton.x + backButton.width / 2 - MeasureText("Back", 20) / 2, backButton.y + backButton.height / 2 - 10, 20, WHITE);

    if (backHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        this->m_sceneManager->ChangeScene(SceneManager::SceneType::MainScene);
    }
}

void AboutScene::Shutdown(entt::registry &registry)
{
}
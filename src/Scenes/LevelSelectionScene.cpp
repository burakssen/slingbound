#include "LevelSelectionScene.h"
#include <iostream>
#include <raymath.h>

void LevelSelectionScene::Initialize(entt::registry &registry)
{
    auto level = registry.create();
    registry.emplace<Level>(level, 1, 5);
}

void LevelSelectionScene::HandleInput(entt::registry &registry)
{
}

void LevelSelectionScene::Update(entt::registry &registry)
{
}

void LevelSelectionScene::Draw(entt::registry &registry)
{
    auto view = registry.view<Level>();
    for (auto entity : view)
    {
        auto &level = view.get<Level>(entity);

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        const int BUTTON_SIZE = 100;
        const int BUTTON_PADDING = 30;
        const int BUTTONS_PER_ROW = 3;

        int totalButtonsWidth = BUTTONS_PER_ROW * BUTTON_SIZE + (BUTTONS_PER_ROW - 1) * BUTTON_PADDING;
        int rows = (level.numberOfLevels + BUTTONS_PER_ROW - 1) / BUTTONS_PER_ROW;
        int totalButtonsHeight = rows * BUTTON_SIZE + (rows - 1) * BUTTON_PADDING;

        int startX = (screenWidth - totalButtonsWidth) / 2;
        int startY = (screenHeight - totalButtonsHeight) / 2;

        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
        DrawRectangleRounded(
            Rectangle{
                static_cast<float>(startX - 20),
                static_cast<float>(startY - 60),
                static_cast<float>(totalButtonsWidth + 40),
                static_cast<float>(totalButtonsHeight + 120)},
            0.1f, 10, BLUE);

        const char *title = TextFormat("LEVEL SELECT");
        int titleWidth = MeasureText(title, 40);
        DrawText(title, (screenWidth - titleWidth) / 2 + 2, startY - 50 + 2, 40, BLACK);
        DrawText(title, (screenWidth - titleWidth) / 2, startY - 50, 40, WHITE);

        const char *progressText = TextFormat("Progress: %d/%d", level.currentLevel, level.numberOfLevels);
        int progressWidth = MeasureText(progressText, 20);
        DrawText(progressText, (screenWidth - progressWidth) / 2, startY - 10, 20, WHITE);

        for (int i = 0; i < level.numberOfLevels; i++)
        {
            int row = i / BUTTONS_PER_ROW;
            int col = i % BUTTONS_PER_ROW;
            float buttonX = startX + col * (BUTTON_SIZE + BUTTON_PADDING);
            float buttonY = startY + row * (BUTTON_SIZE + BUTTON_PADDING) + 30;

            Color buttonColor, textColor;
            bool interactive = false;

            if (level.completedLevels[i] && (i + 1) < level.currentLevel)
            {
                buttonColor = GREEN;
                textColor = WHITE;
                interactive = true;
            }
            else if (!level.completedLevels[i] && (i + 1) == level.currentLevel)
            {
                buttonColor = GOLD;
                textColor = BLACK;
                interactive = true;
            }
            else
            {
                buttonColor = GRAY;
                textColor = BLACK;
                interactive = false;
            }

            Vector2 mousePoint = GetMousePosition();
            bool hover = CheckCollisionPointRec(mousePoint,
                                                Rectangle{buttonX, buttonY, BUTTON_SIZE, BUTTON_SIZE});

            DrawRectangleRounded(
                Rectangle{buttonX, buttonY, BUTTON_SIZE, BUTTON_SIZE},
                0.2f, 10, Fade(BLACK, 0.3f));

            DrawRectangleRounded(
                Rectangle{buttonX - (hover ? 2 : 0), buttonY - (hover ? 2 : 0),
                          static_cast<float>(BUTTON_SIZE + (hover ? 4 : 0)), static_cast<float>(BUTTON_SIZE + (hover ? 4 : 0))},
                0.2f, 10,
                hover ? Fade(buttonColor, 0.8f) : buttonColor);

            const char *buttonText = TextFormat("%d", i + 1);
            int textWidth = MeasureText(buttonText, 40);
            int textHeight = 40;

            DrawText(
                buttonText,
                buttonX + (BUTTON_SIZE - textWidth) / 2,
                buttonY + (BUTTON_SIZE - textHeight) / 2,
                40, textColor);

            if (interactive && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover)
            {
                this->m_sceneManager->ChangeScene(static_cast<SceneManager::SceneType>(i + 1));
            }
        }
    }
}

void LevelSelectionScene::Shutdown(entt::registry &registry)
{
}
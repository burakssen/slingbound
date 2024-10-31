#include "LevelScene.h"

#include <iostream>

void LevelScene::HandleInput(entt::registry &registry)
{
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0)
    {
        m_started = true;
    }

    if (m_started && m_countdownTimer <= 0)
        System::input(registry);
}

void LevelScene::Update(entt::registry &registry)
{
    System::camera(registry, m_camera);
    if (m_started && m_countdownTimer <= 0)
        System::physics(registry);

    auto finishLineView = registry.view<FinishLine, Position>();
    if (finishLineView.begin() != finishLineView.end())
    {
        auto finishLine = finishLineView.front();
        auto &finishLinePos = registry.get<Position>(finishLine);

        auto playerView = registry.view<Player, Position>();
        if (playerView.begin() != playerView.end())
        {
            auto player = playerView.front();
            auto &playerPos = registry.get<Position>(player);

            if (playerPos.x >= finishLinePos.x - 20 && playerPos.x <= finishLinePos.x + 20 && !m_nextLevel)
            {
                m_started = false;
                m_nextLevel = true;

                auto levelView = registry.view<Level>();
                if (levelView.begin() != levelView.end())
                {
                    auto levelEntity = levelView.front();
                    auto &levelComponent = registry.get<Level>(levelEntity);

                    auto soundsView = registry.view<Sounds>();
                    if (soundsView.begin() != soundsView.end())
                    {
                        auto soundsEntity = soundsView.front();
                        auto &sounds = registry.get<Sounds>(soundsEntity);
                        PlaySound(sounds.m_win);
                    }

                    if (levelComponent.currentLevel == levelComponent.numberOfLevels)
                    {
                        levelComponent.message = "Congratulations! You have completed all levels!";
                        m_sceneManager->ChangeScene(SceneManager::SceneType::GameOverScene);
                    }
                    else
                    {

                        levelComponent.completedLevels[levelComponent.currentLevel - 1] = true;
                    }
                }
            }
        }
    }

    auto playerView = registry.view<Player, Position>();
    if (playerView.begin() != playerView.end())
    {
        auto player = playerView.front();
        auto &playerPos = registry.get<Position>(player);

        if (playerPos.y > GetScreenHeight() + 200)
        {
            auto levelView = registry.view<Level>();
            if (levelView.begin() != levelView.end() && !m_nextLevel)
            {
                auto levelEntity = levelView.front();
                auto &levelComponent = registry.get<Level>(levelEntity);
                levelComponent.message = "You fell off the screen!";

                auto soundsView = registry.view<Sounds>();
                if (soundsView.begin() != soundsView.end())
                {
                    auto soundsEntity = soundsView.front();
                    auto &sounds = registry.get<Sounds>(soundsEntity);
                    PlaySound(sounds.m_lose);
                }
            }

            m_sceneManager->ChangeScene(SceneManager::SceneType::GameOverScene);
        }
    }

    if (m_started && m_countdownTimer > 0)
    {
        float currentTime = GetTime();
        if (currentTime - m_lastTime >= 1.0f)
        {
            m_countdownTimer--;
            m_lastTime = currentTime;
        }
    }
}

void LevelScene::Draw(entt::registry &registry)
{
    BeginMode2D(m_camera);
    System::render(registry);
    EndMode2D();

    if (!m_started && !m_nextLevel)
    {
        std::string text = "Press SPACE to start";
        int width = MeasureText(text.c_str(), 60);
        DrawText(text.c_str(), GetScreenWidth() / 2 - width / 2, GetScreenHeight() / 2, 60, GRAY);
    }

    if (m_countdownTimer > 0 && m_started)
    {
        std::string text = std::to_string((int)ceil(m_countdownTimer));
        int width = MeasureText(text.c_str(), 60);
        DrawText(text.c_str(), GetScreenWidth() / 2 - width / 2, GetScreenHeight() / 2, 60, GRAY);
    }

    if (m_nextLevel)
    {
        std::string text = "Level completed!";
        int width = MeasureText(text.c_str(), 60);
        DrawText(text.c_str(), GetScreenWidth() / 2 - width / 2, GetScreenHeight() / 2, 60, GRAY);

        const float BUTTON_WIDTH = 200.0f;
        const float BUTTON_HEIGHT = 80.0f;
        Vector2 mousePoint = GetMousePosition();

        Rectangle nextButton = {
            GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f,
            GetScreenHeight() / 2.0f + 70.0f,
            BUTTON_WIDTH,
            BUTTON_HEIGHT};

        bool nextHover = CheckCollisionPointRec(mousePoint, nextButton);
        Color nextButtonColor = nextHover ? BLUE : GRAY;

        DrawRectangleRounded(nextButton, 0.2f, 10, nextButtonColor);
        DrawText("Next Level ->",
                 nextButton.x + BUTTON_WIDTH / 2 - MeasureText("Next Level ->", 20) / 2,
                 nextButton.y + BUTTON_HEIGHT / 2 - 10,
                 20,
                 WHITE);

        if (nextHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            auto levelView = registry.view<Level>();
            if (levelView.begin() != levelView.end())
            {
                auto levelEntity = levelView.front();
                auto &levelComponent = registry.get<Level>(levelEntity);

                m_nextLevel = false;  // Reset flag after changing level
                m_countdownTimer = 5; // Reset countdown for new level

                m_sceneManager->ChangeScene(static_cast<SceneManager::SceneType>(levelComponent.currentLevel + 1));
            }
        }
    }
}

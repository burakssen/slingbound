#pragma once

#include "SceneManager.h"

#include "Systems.h"

class LevelScene : public IScene
{
public:
    void HandleInput(entt::registry &registry) override;
    void Update(entt::registry &registry) override;
    void Draw(entt::registry &registry) override;

protected:
    Camera2D m_camera = {0};
    bool m_started = false;
    int m_countdownTimer = 5;
    float m_lastTime = 0.0f;
    bool m_nextLevel = false;
};
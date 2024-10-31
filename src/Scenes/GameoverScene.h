#pragma once

#include "SceneManager.h"

#include "Tags.h"

class GameoverScene : public IScene
{
public:
    void Initialize(entt::registry &registry) override;
    void HandleInput(entt::registry &registry) override;
    void Update(entt::registry &registry) override;
    void Draw(entt::registry &registry) override;
    void Shutdown(entt::registry &registry) override;
};
#pragma once

#include "LevelScene.h"

class Level5Scene : public LevelScene
{
public:
    void Initialize(entt::registry &registry) override;
    void Shutdown(entt::registry &registry) override;

private:
    std::vector<entt::entity> m_sceneEntities;
};

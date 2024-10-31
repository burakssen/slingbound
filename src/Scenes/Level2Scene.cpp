#include "Level2Scene.h"

void Level2Scene::Initialize(entt::registry &registry)
{
    auto levelView = registry.view<Level>();
    if (levelView.begin() != levelView.end())
    {
        auto levelEntity = levelView.front();
        auto &levelComponent = registry.get<Level>(levelEntity);
        levelComponent.currentLevel = 2;
    }

    this->m_camera.target = {0.0f, 0.0f};
    this->m_camera.offset = {static_cast<float>(GetScreenWidth()) / 8.0f, 0};
    this->m_camera.zoom = 1.0f;
    this->m_camera.rotation = 0.0f;

    // Create player entity
    auto player = registry.create();
    registry.emplace<Player>(player);
    registry.emplace<Position>(player, 100.0f, 100.0f);
    registry.emplace<Velocity>(player, 0.0f, 0.0f);
    this->m_sceneEntities.emplace_back(player);

    // Define static anchors
    std::vector<std::pair<Position, AnchorType>> anchorPoints = {
        {{200.0f, 250.0f}, AnchorType::Static},
        {{600.0f, 400.0f}, AnchorType::Static},
        {{800.0f, 450.0f}, AnchorType::Static},
        {{1000.0f, 300.0f}, AnchorType::Static},
        {{1200.0f, 350.0f}, AnchorType::Static},
    };

    for (const auto &[pos, type] : anchorPoints)
    {
        auto anchor = registry.create();
        registry.emplace<Anchor>(anchor);
        registry.emplace<AnchorType>(anchor, type);
        registry.emplace<Position>(anchor, pos.x, pos.y);
        this->m_sceneEntities.emplace_back(anchor);
    }

    // Define static obstacles
    std::vector<std::tuple<Vector2, Position, ObstacleType>> obstacles = {
        {{100.0f, 20.0f}, {300.0f, 400.0f}, ObstacleType::Static},
        {{150.0f, 20.0f}, {500.0f, 250.0f}, ObstacleType::Static},
        {{120.0f, 30.0f}, {700.0f, 350.0f}, ObstacleType::Static},
    };

    for (const auto &[size, pos, type] : obstacles)
    {
        auto obstacle = registry.create();
        registry.emplace<Obstacle>(obstacle, size, false, 0.0f, 0.0f);
        registry.emplace<Position>(obstacle, pos.x, pos.y);
        this->m_sceneEntities.emplace_back(obstacle);
    }

    // Add a finish line
    auto finishLine = registry.create();
    registry.emplace<FinishLine>(finishLine);
    registry.emplace<Position>(finishLine, 1400.0f, 600.0f);
    this->m_sceneEntities.emplace_back(finishLine);
}

void Level2Scene::Shutdown(entt::registry &registry)
{
    for (auto entity : m_sceneEntities)
    {
        registry.destroy(entity);
    }

    this->m_sceneEntities.clear();
    this->m_started = false;
    this->m_countdownTimer = 5;
}

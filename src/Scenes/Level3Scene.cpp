#include "Level3Scene.h"

void Level3Scene::Initialize(entt::registry &registry)
{
    auto levelView = registry.view<Level>();
    if (levelView.begin() != levelView.end())
    {
        auto levelEntity = levelView.front();
        auto &levelComponent = registry.get<Level>(levelEntity);
        levelComponent.currentLevel = 3;
    }

    this->m_camera.target = {0.0f, 0.0f};
    this->m_camera.offset = {static_cast<float>(GetScreenWidth()) / 8.0f, 0};
    this->m_camera.zoom = 1.0f;
    this->m_camera.rotation = 0.0f;

    auto player = registry.create();
    registry.emplace<Player>(player);
    registry.emplace<Position>(player, 100.0f, 100.0f);
    registry.emplace<Velocity>(player, 0.0f, 0.0f);
    this->m_sceneEntities.emplace_back(player);

    std::vector<std::tuple<AnchorType, Position>> anchors = {
        std::make_tuple(AnchorType::Static, Position{250.0f, 150.0f}),
    };

    for (auto anchorInfo : anchors)
    {
        auto anchor = registry.create();
        registry.emplace<Anchor>(anchor);
        registry.emplace<AnchorType>(anchor, std::get<0>(anchorInfo));
        registry.emplace<Position>(anchor, std::get<1>(anchorInfo).x, std::get<1>(anchorInfo).y);
        this->m_sceneEntities.emplace_back(anchor);
    }

    std::vector<std::tuple<Obstacle, Position>> obstacles = {
        std::make_tuple(Obstacle{Vector2{10.0f, 150.0f}, false, 0.0f, 0.0f}, Position{375.0f, 150.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 300.0f}, false, 0.0f, 0.0f}, Position{375.0f, 500.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 150.0f}, true, 2.0f, 90.0f}, Position{500.0f, 500.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 150.0f}, true, 2.0f, 135.0f}, Position{700.0f, 575.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 150.0f}, true, 2.0f, 65.0f}, Position{900.0f, 575.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 150.0f}, true, 2.0f, 90.0f}, Position{1100.0f, 500.0f}),
    };

    for (auto obstacleInfo : obstacles)
    {
        auto obstacle = registry.create();
        registry.emplace<Obstacle>(obstacle, std::get<0>(obstacleInfo));
        registry.emplace<Position>(obstacle, std::get<1>(obstacleInfo).x, std::get<1>(obstacleInfo).y);
        this->m_sceneEntities.emplace_back(obstacle);
    }

    auto finishLine = registry.create();
    registry.emplace<FinishLine>(finishLine);
    registry.emplace<Position>(finishLine, 1400.0f, 600.0f);
    this->m_sceneEntities.emplace_back(finishLine);
}

void Level3Scene::Shutdown(entt::registry &registry)
{
    for (auto entity : m_sceneEntities)
    {
        registry.destroy(entity);
    }

    this->m_sceneEntities.clear();
    this->m_started = false;
    this->m_countdownTimer = 5;
}

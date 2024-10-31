#include "Level5Scene.h"

void Level5Scene::Initialize(entt::registry &registry)
{
    auto levelView = registry.view<Level>();
    if (levelView.begin() != levelView.end())
    {
        auto levelEntity = levelView.front();
        auto &levelComponent = registry.get<Level>(levelEntity);
        levelComponent.currentLevel = 5;
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

    std::vector<std::tuple<AnchorType, Position, std::optional<std::pair<Vector2, Vector2>>>> anchors = {
        std::make_tuple(AnchorType::Static, Position{250.0f, 350.0f}, std::nullopt),
        std::make_tuple(AnchorType::Dynamic, Position{1275.0f, 600.0f}, std::make_pair(Vector2{1275.0f, 600.0f}, Vector2{1600.0f, 200.0f})),
        std::make_tuple(AnchorType::SlingShot, Position{1850.0f, 300.0f}, std::nullopt),
    };

    for (auto anchorInfo : anchors)
    {
        auto anchor = registry.create();

        registry.emplace<AnchorType>(anchor, std::get<0>(anchorInfo));
        if (std::get<0>(anchorInfo) == AnchorType::Dynamic)
            registry.emplace<Anchor>(anchor, true, std::get<2>(anchorInfo).value().first, std::get<2>(anchorInfo).value().second, 0);
        else
            registry.emplace<Anchor>(anchor);

        registry.emplace<Position>(anchor, std::get<1>(anchorInfo).x, std::get<1>(anchorInfo).y);
        this->m_sceneEntities.emplace_back(anchor);
    }

    std::vector<std::tuple<Obstacle, Position>> obstacles = {
        std::make_tuple(Obstacle{Vector2{10.0f, 200.0f}, true, 2.0f, 120.0f}, Position{1050.0f, 600.0f}),
        std::make_tuple(Obstacle{Vector2{10.0f, 200.0f}, true, 2.0f, 60.0f}, Position{1150.0f, 600.0f}),
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
    registry.emplace<Position>(finishLine, 2550.0f, 600.0f);
    this->m_sceneEntities.emplace_back(finishLine);
}

void Level5Scene::Shutdown(entt::registry &registry)
{
    for (auto entity : m_sceneEntities)
    {
        registry.destroy(entity);
    }

    this->m_sceneEntities.clear();
    this->m_started = false;
    this->m_countdownTimer = 5;
}

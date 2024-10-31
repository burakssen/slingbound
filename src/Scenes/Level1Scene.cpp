#include "Level1Scene.h"

void Level1Scene::Initialize(entt::registry &registry)
{
    auto levelView = registry.view<Level>();
    if (levelView.begin() != levelView.end())
    {
        auto levelEntity = levelView.front();
        auto &levelComponent = registry.get<Level>(levelEntity);
        levelComponent.currentLevel = 1;
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
        std::make_tuple(AnchorType::Static, Position{475.0f, 200.0f}),
        std::make_tuple(AnchorType::Static, Position{700.0f, 250.0f}),
        std::make_tuple(AnchorType::Static, Position{925.0f, 300.0f}),
        std::make_tuple(AnchorType::Static, Position{1150.0f, 350.0f}),
    };

    for (auto anchorInfo : anchors)
    {
        auto anchor = registry.create();
        registry.emplace<Anchor>(anchor);
        registry.emplace<AnchorType>(anchor, std::get<0>(anchorInfo));
        registry.emplace<Position>(anchor, std::get<1>(anchorInfo).x, std::get<1>(anchorInfo).y);
        this->m_sceneEntities.emplace_back(anchor);
    }

    auto finishLine = registry.create();
    registry.emplace<FinishLine>(finishLine);
    registry.emplace<Position>(finishLine, 1400.0f, 600.0f);
}

void Level1Scene::Shutdown(entt::registry &registry)
{
    for (auto entity : m_sceneEntities)
    {
        registry.destroy(entity);
    }

    this->m_sceneEntities.clear();
    this->m_started = false;
    this->m_countdownTimer = 5;
}

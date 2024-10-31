#pragma once

#include "SceneManager.h"

#include <raylib.h>

class AboutScene : public IScene
{
public:
    void Initialize(entt::registry &registry) override;
    void HandleInput(entt::registry &registry) override;
    void Update(entt::registry &registry) override;
    void Draw(entt::registry &registry) override;
    void Shutdown(entt::registry &registry) override;

private:
    std::string info = "Built using Raylib+EnTT";
    std::string by = "By @burakssen";
    std::array<Color, 8> colors = {BLACK,
                                   GRAY,
                                   WHITE,
                                   GREEN,
                                   RED,
                                   BLUE,
                                   GOLD,
                                   PURPLE};
};
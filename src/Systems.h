#pragma once

#include <unordered_map>
#include <raylib.h>
#include <entt/entt.hpp>
#include "Tags.h"
#include "Constants.h"
#include "Utils.h"

namespace System
{
    void render(entt::registry &registry);
    void input(entt::registry &registry);
    void physics(entt::registry &registry);
    void camera(entt::registry &registry, Camera2D &camera);
}

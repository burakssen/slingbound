#pragma once

#include <raylib.h>
#include <entt/entt.hpp>

#include <functional>

enum class AnchorType
{
    Static,
    Dynamic,
    SlingShot
};

enum class ObstacleType
{
    Static,
    Bouncy
};

struct Player
{
};

struct Anchor
{
    bool m_is_dynamic = false;
    Vector2 m_point1 = {0, 0};
    Vector2 m_point2 = {0, 0};
    int m_next_point = 0;
};

struct Obstacle
{
    Vector2 m_size = {0, 0};
    bool m_bounce = false;
    float m_bounce_factor = 0.0f;
    float m_angle = 0.0f;
};

struct Rope
{
    entt::entity m_anchor;
    entt::entity m_player;
    float m_length;
    bool m_is_grappling;
};

struct Velocity
{
    float x;
    float y;
};

struct Position
{
    float x;
    float y;
};

struct Level
{
    int currentLevel;
    int numberOfLevels;
    std::array<bool, 5> completedLevels = {false, false, false, false, false};
    std::string message = "";
};

struct FinishLine
{
};

struct Sounds
{
    Sound m_sling = LoadSound("resources/sling.wav");
    Sound m_hit = LoadSound("resources/hit.wav");
    Sound m_win = LoadSound("resources/win.wav");
    Sound m_lose = LoadSound("resources/lose.wav");
};
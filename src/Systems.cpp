#include "Systems.h"
#include <iostream>
#include <stdexcept>

namespace System
{
    bool check_ray_rect_intersection(
        const Position &ray_start, const Position &ray_end,
        const Position &rect_pos, const Vector2 &rect_size, float rect_angle)
    {
        float s = sin(-rect_angle * DEG2RAD), c = cos(-rect_angle * DEG2RAD);

        // Transform ray to rectangle's local space
        Position local_start = {
            (ray_start.x - rect_pos.x) * c - (ray_start.y - rect_pos.y) * s,
            (ray_start.x - rect_pos.x) * s + (ray_start.y - rect_pos.y) * c};

        Position local_end = {
            (ray_end.x - rect_pos.x) * c - (ray_end.y - rect_pos.y) * s,
            (ray_end.x - rect_pos.x) * s + (ray_end.y - rect_pos.y) * c};

        // AABB intersection in local space
        float tx1 = (-rect_size.x / 2 - local_start.x) / (local_end.x - local_start.x);
        float tx2 = (rect_size.x / 2 - local_start.x) / (local_end.x - local_start.x);
        float ty1 = (-rect_size.y / 2 - local_start.y) / (local_end.y - local_start.y);
        float ty2 = (rect_size.y / 2 - local_start.y) / (local_end.y - local_start.y);

        float tmin = std::max(std::min(tx1, tx2), std::min(ty1, ty2));
        float tmax = std::min(std::max(tx1, tx2), std::max(ty1, ty2));

        return tmax >= tmin && tmax >= 0 && tmin <= 1;
    }

    bool check_circle_rect_collision(
        const Position &circle_pos, float circle_radius,
        const Position &rect_pos, const Vector2 &rect_size, float rect_angle)
    {
        float s = sin(-rect_angle * DEG2RAD), c = cos(-rect_angle * DEG2RAD);
        float rel_x = circle_pos.x - rect_pos.x, rel_y = circle_pos.y - rect_pos.y;

        // Rotate point to align with rectangle
        float rot_x = rel_x * c - rel_y * s, rot_y = rel_x * s + rel_y * c;

        // Find closest point on rectangle
        float closest_x = std::clamp(rot_x, -rect_size.x / 2, rect_size.x / 2);
        float closest_y = std::clamp(rot_y, -rect_size.y / 2, rect_size.y / 2);

        float dx = rot_x - closest_x, dy = rot_y - closest_y;
        return sqrt(dx * dx + dy * dy) <= circle_radius;
    }

    // Helper function to find the time of collision
    float find_collision_time(
        const Position &ray_start, const Position &ray_end,
        const Position &rect_pos, const Vector2 &rect_size, float rect_angle)
    {
        const float COLLISION_EPSILON = 0.01f;
        float left = 0.0f;
        float right = 1.0f;

        // Binary search for collision time
        for (int i = 0; i < 10; i++)
        {
            float mid = (left + right) / 2;
            Position test_pos = {
                ray_start.x + (ray_end.x - ray_start.x) * mid,
                ray_start.y + (ray_end.y - ray_start.y) * mid};

            if (check_circle_rect_collision(test_pos, PLAYER_RADIUS,
                                            rect_pos, rect_size, rect_angle))
            {
                right = mid;
            }
            else
            {
                left = mid;
            }
        }

        return left - COLLISION_EPSILON;
    }

    entt::entity get_closest_anchor(entt::registry &registry, const Position &player_pos)
    {
        entt::entity closest = entt::null;
        float min_dist = std::numeric_limits<float>::max();

        for (const auto [entity, pos, type] : registry.view<Position, AnchorType>().each())
        {
            if (type != AnchorType::Static && type != AnchorType::SlingShot && type != AnchorType::Dynamic)
                continue;

            float dx = player_pos.x - pos.x, dy = player_pos.y - pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < min_dist)
            {
                min_dist = dist;
                closest = entity;
            }
        }

        return closest;
    }

    void resolve_collision(
        Position &circle_pos, Velocity &circle_vel,
        const Position &rect_pos, const Vector2 &rect_size, float rect_angle,
        bool is_bouncy = true)
    {
        float s = sin(-rect_angle * DEG2RAD), c = cos(-rect_angle * DEG2RAD);
        float rel_x = circle_pos.x - rect_pos.x, rel_y = circle_pos.y - rect_pos.y;

        float rot_x = rel_x * c - rel_y * s, rot_y = rel_x * s + rel_y * c;
        float closest_x = std::clamp(rot_x, -rect_size.x / 2, rect_size.x / 2);
        float closest_y = std::clamp(rot_y, -rect_size.y / 2, rect_size.y / 2);

        float dx = rot_x - closest_x, dy = rot_y - closest_y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 0)
        {
            // Calculate normal vector in rotated space
            float nx = dx / distance, ny = dy / distance;

            // Rotate normal vector back to world space
            float world_nx = nx * c + ny * s;
            float world_ny = -nx * s + ny * c;

            // Move circle out of collision
            float penetration = PLAYER_RADIUS - distance;
            circle_pos.x += world_nx * penetration;
            circle_pos.y += world_ny * penetration;

            // Calculate reflection using proper bounce factor
            float dot = circle_vel.x * world_nx + circle_vel.y * world_ny;
            float bounce_factor = is_bouncy ? 1.0f : 0.2f;

            // Only reflect velocity component that's moving into the surface
            if (dot < 0)
            {
                circle_vel.x = circle_vel.x - (1.0f + bounce_factor) * dot * world_nx;
                circle_vel.y = circle_vel.y - (1.0f + bounce_factor) * dot * world_ny;

                // Apply minimum velocity threshold for bouncy collisions
                if (is_bouncy)
                {
                    float vel_magnitude = sqrt(circle_vel.x * circle_vel.x +
                                               circle_vel.y * circle_vel.y);
                    if (vel_magnitude < 1.0f)
                    {
                        circle_vel.x = 0;
                        circle_vel.y = 0;
                    }
                }
            }
        }
    }

    void update_rope_physics(Position &player_pos, Velocity &player_vel,
                             const Position &anchor_pos, float rope_length, bool is_slingshot)
    {
        if (rope_length <= 0.0f)
            throw std::invalid_argument("Invalid rope length");

        float dx = player_pos.x - anchor_pos.x, dy = player_pos.y - anchor_pos.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < std::numeric_limits<float>::epsilon())
            return;

        if (is_slingshot)
        {
            // More controlled slingshot acceleration
            float max_slingshot_force = 500.0f; // Adjustable slingshot power
            float frame_time = GetFrameTime();

            // Calculate direction away from anchor
            float dir_x = dx / dist, dir_y = dy / dist;

            // Apply force proportional to stretch
            float stretch_factor = std::min(dist / rope_length, 2.0f);
            float force = max_slingshot_force * stretch_factor * frame_time;

            player_vel.x -= dir_x * force;
            player_vel.y -= dir_y * force;

            return;
        }

        if (dist > rope_length)
        {
            float dir_x = dx / dist, dir_y = dy / dist;
            float excess = dist - rope_length;
            player_pos.x -= dir_x * excess;
            player_pos.y -= dir_y * excess;

            float perp_x = -dir_y, perp_y = dir_x;
            float tang_vel = player_vel.x * perp_x + player_vel.y * perp_y;
            player_vel = {perp_x * tang_vel, perp_y * tang_vel};
        }
    }

    void render(entt::registry &registry)
    {
        // Draw anchors
        auto draw_anchors = [](const Position &pos, Color color)
        {
            BeginBlendMode(BLEND_ALPHA);
            for (int i = 0; i < 5; i++)
                DrawCircle(pos.x, pos.y, 10 + i * 2, Fade(color, 0.2f));
            DrawCircle(pos.x, pos.y, 10, color);
            EndBlendMode();
        };

        // 1. Anchors
        for (const auto [entity, pos, anchor_type] : registry.view<Position, AnchorType>().each())
        {
            if (!registry.valid(entity))
                continue;
            Color color = anchor_type == AnchorType::Static ? BLUE : anchor_type == AnchorType::Dynamic ? GOLD
                                                                                                        : PURPLE;
            draw_anchors(pos, color);
        }

        // Draw indicators for closest anchor
        auto player_view = registry.view<Player, Position>();
        if (player_view.begin() != player_view.end())
        {
            auto player = player_view.front();
            if (registry.valid(player))
            {
                const auto &player_pos = registry.get<Position>(player);
                auto closest = get_closest_anchor(registry, player_pos);
                if (registry.valid(closest))
                {
                    const auto &closest_pos = registry.get<Position>(closest);
                    // Check if player is in range of closest anchor
                    float dx = player_pos.x - closest_pos.x, dy = player_pos.y - closest_pos.y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < MAX_ROPE_DISTANCE)
                    {
                        DrawCircleLines(closest_pos.x, closest_pos.y, 30, GRAY);
                        // Clip circle lines on four sides
                        DrawRectangle(closest_pos.x - 7.5f, closest_pos.y - 40, 15, 15, BLACK);
                        DrawRectangle(closest_pos.x - 7.5f, closest_pos.y + 20, 15, 15, BLACK);
                        DrawRectangle(closest_pos.x - 40, closest_pos.y - 7.5f, 15, 15, BLACK);
                        DrawRectangle(closest_pos.x + 20, closest_pos.y - 7.5f, 15, 15, BLACK);
                    }
                }
            }
        }

        // 2. Ropes
        for (const auto [entity, rope] : registry.view<Rope>().each())
        {
            if (!registry.valid(rope.m_anchor) || !registry.valid(rope.m_player))
                continue;
            const auto &anchor_pos = registry.get<Position>(rope.m_anchor);
            const auto &player_pos = registry.get<Position>(rope.m_player);
            DrawLineEx({anchor_pos.x, anchor_pos.y}, {player_pos.x, player_pos.y}, 3, GREEN);
        }

        // 3. Obstacles
        for (const auto [entity, pos, obstacle] : registry.view<Position, Obstacle>().each())
        {
            if (!registry.valid(entity))
                continue;
            DrawRectanglePro(
                {pos.x, pos.y, obstacle.m_size.x, obstacle.m_size.y},
                {obstacle.m_size.x / 2, obstacle.m_size.y / 2},
                obstacle.m_angle,
                obstacle.m_bounce ? GOLD : RED);
        }

        // 4. Finish Line
        for (const auto [entity, pos] : registry.view<Position, FinishLine>().each())
        {
            if (!registry.valid(entity))
                continue;
            DrawRectangle(pos.x - 20, 0, 20, GetScreenHeight(), GOLD);
        }

        // 5. Player
        for (const auto [entity, pos] : registry.view<Position, Player>().each())
        {
            if (!registry.valid(entity))
                continue;
            DrawCircle(pos.x, pos.y, PLAYER_RADIUS, RED);
        }
    }

    void physics(entt::registry &registry)
    {
        auto player_view = registry.view<Player, Position, Velocity>();
        if (player_view.begin() == player_view.end())
            return;

        auto player = player_view.front();
        if (!registry.valid(player))
            return;

        auto &pos = registry.get<Position>(player);
        auto &vel = registry.get<Velocity>(player);

        const float FIXED_TIME_STEP = 1 / 360.0f;
        float dt = GetFrameTime();

        static float accumulatedTime = 0.0f;
        accumulatedTime += dt;

        while (accumulatedTime >= FIXED_TIME_STEP)
        {
            vel.y += GRAVITY * FIXED_TIME_STEP * 2000;

            // Apply rope physics
            for (auto [entity, rope] : registry.view<Rope>().each())
            {
                if (!registry.valid(rope.m_anchor))
                    continue;
                const auto &anchor_pos = registry.get<Position>(rope.m_anchor);
                update_rope_physics(pos, vel, anchor_pos, rope.m_length, rope.m_is_grappling);
            }

            // Predict new position
            Position new_pos = {
                pos.x + vel.x * FIXED_TIME_STEP,
                pos.y + vel.y * FIXED_TIME_STEP};

            // Fast-moving collision checks
            Position prev_pos = pos;
            bool had_collision = false;

            for (const auto [entity, obstacle, obstacle_pos] : registry.view<Obstacle, Position>().each())
            {
                if (!registry.valid(entity))
                    continue;

                // Check both current position and swept path
                bool current_collision = check_circle_rect_collision(new_pos, PLAYER_RADIUS,
                                                                     obstacle_pos, obstacle.m_size,
                                                                     obstacle.m_angle);

                bool swept_collision = check_ray_rect_intersection(prev_pos, new_pos,
                                                                   obstacle_pos, obstacle.m_size,
                                                                   obstacle.m_angle);

                if (current_collision || swept_collision)
                {
                    auto soundsView = registry.view<Sounds>();
                    if (soundsView.begin() != soundsView.end())
                    {
                        auto soundsEntity = soundsView.front();
                        auto &soundsComponent = registry.get<Sounds>(soundsEntity);
                        PlaySound(soundsComponent.m_hit);
                    }

                    had_collision = true;

                    // For high-speed collisions, find precise collision point
                    if (swept_collision)
                    {
                        float t = find_collision_time(prev_pos, new_pos, obstacle_pos,
                                                      obstacle.m_size, obstacle.m_angle);
                        new_pos = {
                            prev_pos.x + (new_pos.x - prev_pos.x) * t,
                            prev_pos.y + (new_pos.y - prev_pos.y) * t};
                    }

                    // Modified collision resolution that preserves momentum
                    float s = sin(-obstacle.m_angle * DEG2RAD);
                    float c = cos(-obstacle.m_angle * DEG2RAD);
                    float rel_x = new_pos.x - obstacle_pos.x;
                    float rel_y = new_pos.y - obstacle_pos.y;
                    float rot_x = rel_x * c - rel_y * s;
                    float rot_y = rel_x * s + rel_y * c;
                    float closest_x = std::clamp(rot_x, -obstacle.m_size.x / 2, obstacle.m_size.x / 2);
                    float closest_y = std::clamp(rot_y, -obstacle.m_size.y / 2, obstacle.m_size.y / 2);
                    float dx = rot_x - closest_x;
                    float dy = rot_y - closest_y;
                    float dist = sqrt(dx * dx + dy * dy);

                    if (dist > 0)
                    {
                        // Calculate normal in world space
                        float nx = (dx / dist) * c + (dy / dist) * s;
                        float ny = -(dx / dist) * s + (dy / dist) * c;

                        // Move out of collision
                        float penetration = PLAYER_RADIUS - dist;
                        new_pos.x += nx * penetration;
                        new_pos.y += ny * penetration;

                        // Reflect velocity off surface while preserving magnitude
                        float dot = vel.x * nx + vel.y * ny;
                        if (dot < 0)
                        {
                            float speed = sqrt(vel.x * vel.x + vel.y * vel.y);
                            // For bouncy obstacles, maintain full speed
                            if (obstacle.m_bounce)
                            {
                                vel.x = vel.x - 2 * dot * nx;
                                vel.y = vel.y - 2 * dot * ny;
                            }
                            else
                            {
                                // For non-bouncy obstacles, redirect along the surface
                                // while maintaining most of the speed
                                float tx = -ny; // Tangent vector
                                float ty = nx;
                                float tang_dot = vel.x * tx + vel.y * ty;
                                vel.x = tx * tang_dot * 0.95f; // Preserve 95% of speed along surface
                                vel.y = ty * tang_dot * 0.95f;
                            }
                            // Ensure velocity magnitude is maintained
                            float new_speed = sqrt(vel.x * vel.x + vel.y * vel.y);
                            if (new_speed > 0)
                            {
                                vel.x = vel.x * speed / new_speed;
                                vel.y = vel.y * speed / new_speed;
                            }
                        }
                    }
                    break;
                }
            }

            pos = new_pos;

            accumulatedTime -= FIXED_TIME_STEP;
        }

        // Update dynamic anchors
        for (auto [entity, anchor, pos, type] : registry.view<Anchor, Position, AnchorType>().each())
        {
            if (type != AnchorType::Dynamic)
                continue;

            Vector2 target = anchor.m_next_point == 0 ? anchor.m_point1 : anchor.m_point2;
            float dx = target.x - pos.x, dy = target.y - pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist > 1.0f)
            {
                // Smoothly interpolate movement
                float move_factor = std::min(DYNAMIC_ANCHOR_SPEED / dist, 1.0f);
                registry.replace<Position>(entity,
                                           pos.x + dx * move_factor,
                                           pos.y + dy * move_factor);
            }
            else
            {
                // Switch points when very close to the target
                anchor.m_next_point = 1 - anchor.m_next_point;
            }
        }
    }

    void input(entt::registry &registry)
    {
        auto player_view = registry.view<Player, Position>();
        if (player_view.begin() == player_view.end())
            return;

        auto player = player_view.front();
        if (!registry.valid(player))
            return;

        const auto &player_pos = registry.get<Position>(player);

        bool is_input_triggered =
            IsKeyPressed(KEY_SPACE) ||                 // Keyboard
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || // Mouse
            GetTouchPointCount() > 0;                  // Touch

        if (is_input_triggered)
        {
            auto anchor = get_closest_anchor(registry, player_pos);

            if (anchor == entt::null || !registry.valid(anchor))
                return;

            const auto &anchor_pos = registry.get<Position>(anchor);
            float dist = Utils::distance(player_pos, anchor_pos);

            if (dist > MAX_ROPE_DISTANCE)
                return;

            auto &player_vel = registry.get<Velocity>(player);
            auto anchor_type_ptr = registry.try_get<AnchorType>(anchor);
            bool is_slingshot = anchor_type_ptr && *anchor_type_ptr == AnchorType::SlingShot;

            float dx = anchor_pos.x - player_pos.x;
            float dy = anchor_pos.y - player_pos.y;
            float normalized_dist = std::sqrt(dx * dx + dy * dy);

            if (normalized_dist > std::numeric_limits<float>::epsilon())
            {
                // Base velocity boost reduced for more control
                float base_boost = is_slingshot ? 15.0f : 10.0f;
                player_vel.x += dx / normalized_dist * base_boost;
                player_vel.y += dy / normalized_dist * base_boost;

                auto rope = registry.create();
                registry.emplace<Rope>(rope, anchor, player, dist, is_slingshot);

                auto soundsView = registry.view<Sounds>();
                if (soundsView.begin() != soundsView.end())
                {
                    auto soundsEntity = soundsView.front();
                    auto &sounds = registry.get<Sounds>(soundsEntity);
                    PlaySound(sounds.m_sling);
                }
            }
        }

        // Clear rope on input release
        bool is_input_released =
            IsKeyReleased(KEY_SPACE) ||
            IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        if (is_input_released)
        {
            registry.clear<Rope>();
            return;
        }
    }

    void camera(entt::registry &registry, Camera2D &camera)
    {
        auto player_view = registry.view<Player, Position>();
        if (player_view.begin() == player_view.end() || !registry.valid(player_view.front()))
            return;

        const auto &player_pos = registry.get<Position>(player_view.front());
        camera.target.x += (player_pos.x - camera.target.x) * 0.02f;
    }
}
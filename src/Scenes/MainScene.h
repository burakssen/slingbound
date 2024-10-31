#pragma once

#include "SceneManager.h"
#include "Constants.h"

#include <raylib.h>
#include <raymath.h>

#include <vector>

class MainScene : public IScene
{
public:
    void Initialize(entt::registry &registry) override;
    void HandleInput(entt::registry &registry) override;
    void Update(entt::registry &registry) override;
    void Draw(entt::registry &registry) override;
    void Shutdown(entt::registry &registry) override;

private:
    struct Animation
    {
        Vector2 anchorPosition;
        Vector2 playerPosition;
        float angle;
        float angularVelocity;
        float ropeLength;
        float frequency;

        Animation(Vector2 anchorPos, float angle, float ropeLength, float frequency = 3.0f)
            : anchorPosition(anchorPos), angle(angle), angularVelocity(0), ropeLength(ropeLength), frequency(frequency)
        {
            playerPosition = {
                anchorPos.x + ropeLength * sinf(angle),
                anchorPos.y + ropeLength * cosf(angle)};
        }

        void UpdatePendulum(float deltaTime)
        {
            const float gravity = 9.81f;                                          // Gravity constant
            const float swingAcceleration = -frequency * frequency * sinf(angle); // Angular acceleration

            // Update angular velocity and angle
            angularVelocity += swingAcceleration * deltaTime;
            angle += angularVelocity * deltaTime;

            // Update player position based on new angle
            playerPosition = {
                anchorPosition.x + ropeLength * sinf(angle),
                anchorPosition.y + ropeLength * cosf(angle)};
        }
    };

    std::vector<Animation> m_animations;
};

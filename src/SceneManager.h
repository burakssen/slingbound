#pragma once
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

class SceneManager;

class IScene
{
public:
    virtual ~IScene() = default;
    virtual void Initialize(entt::registry &registry) = 0;
    virtual void HandleInput(entt::registry &registry) = 0;
    virtual void Update(entt::registry &registry) = 0;
    virtual void Draw(entt::registry &registry) = 0;
    virtual void Shutdown(entt::registry &registry) = 0;

    void SetSceneManager(SceneManager *sceneManager)
    {
        m_sceneManager = sceneManager;
    }

protected:
    SceneManager *m_sceneManager = nullptr;
};

class SceneManager
{
public:
    explicit SceneManager(entt::registry &registry) : m_registry(registry) {}
    enum class SceneType
    {
        MainScene = 0,
        Level1Scene = 1,
        Level2Scene = 2,
        Level3Scene = 3,
        Level4Scene = 4,
        Level5Scene = 5,
        LevelSelectionScene = 6,
        GameOverScene = 7,
        AboutScene = 8
    };

    void ChangeScene(SceneType newScene);
    void HandleInput();
    void Update();
    void Draw();

    void RegisterScene(SceneType sceneType, std::shared_ptr<IScene> scene);

private:
    std::unordered_map<SceneType, std::shared_ptr<IScene>> m_scenes;
    std::shared_ptr<IScene> m_currentScene;
    entt::registry &m_registry;
};

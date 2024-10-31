#pragma once

#include <string>
#include <raylib.h>
#include <entt/entt.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Tags.h"
#include "Systems.h"

#include "SceneManager.h"

#include "Scenes/MainScene.h"
#include "Scenes/GameoverScene.h"
#include "Scenes/LevelSelectionScene.h"
#include "Scenes/Level1Scene.h"
#include "Scenes/Level2Scene.h"
#include "Scenes/Level3Scene.h"
#include "Scenes/Level4Scene.h"
#include "Scenes/Level5Scene.h"
#include "Scenes/AboutScene.h"

class Game
{
private:
    Game();
    ~Game();

public:
    static void Initialize();
    static Game &GetInstance();
    static void Shutdown();

    void Run();

private:
#ifdef __EMSCRIPTEN__
    static void MainLoop();
#endif

    void HandleInput();
    void Update();
    void Draw();

private:
    inline static Game *s_instance = nullptr;

    Vector2 m_size = {1280, 720};
    std::string m_title = "Slingbound";
    entt::registry m_registry;

    std::unique_ptr<SceneManager> m_sceneManager;
};
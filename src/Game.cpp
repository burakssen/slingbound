#include "Game.h"

Game::Game()
    : m_sceneManager(std::make_unique<SceneManager>(this->m_registry))
{
    InitWindow(this->m_size.x, this->m_size.y, this->m_title.c_str());
    SetTargetFPS(60);

    InitAudioDevice();

    auto sounds = this->m_registry.create();
    this->m_registry.emplace<Sounds>(sounds);

    this->m_sceneManager->RegisterScene(SceneManager::SceneType::MainScene, std::make_shared<MainScene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::GameOverScene, std::make_shared<GameoverScene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::LevelSelectionScene, std::make_shared<LevelSelectionScene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::Level1Scene, std::make_shared<Level1Scene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::Level2Scene, std::make_shared<Level2Scene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::Level3Scene, std::make_shared<Level3Scene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::Level4Scene, std::make_shared<Level4Scene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::Level5Scene, std::make_shared<Level5Scene>());
    this->m_sceneManager->RegisterScene(SceneManager::SceneType::AboutScene, std::make_shared<AboutScene>());
    this->m_sceneManager->ChangeScene(SceneManager::SceneType::MainScene);
}

Game::~Game()
{
    CloseWindow();
}

void Game::Initialize()
{
    if (Game::s_instance != nullptr)
    {
        throw std::runtime_error("Game already initialized");
    }

    Game::s_instance = new Game();
}

Game &Game::GetInstance()
{
    if (Game::s_instance == nullptr)
    {
        throw std::runtime_error("Game not initialized");
    }

    return *Game::s_instance;
}

void Game::Shutdown()
{
    if (Game::s_instance == nullptr)
    {
        throw std::runtime_error("Game not initialized");
    }

    delete Game::s_instance;
    Game::s_instance = nullptr;
}

void Game::Run()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(MainLoop, 0, 1);
#else
    while (!WindowShouldClose())
    {
        this->HandleInput();
        this->Update();
        this->Draw();
    }
#endif
}

#ifdef __EMSCRIPTEN__
void Game::MainLoop()
{
    Game &game = Game::GetInstance();
    game.HandleInput();
    game.Update();
    game.Draw();
}

#endif

void Game::HandleInput()
{
    this->m_sceneManager->HandleInput();
}

void Game::Update()
{
    this->m_sceneManager->Update();
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    this->m_sceneManager->Draw();
    EndDrawing();
}
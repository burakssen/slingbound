#include "SceneManager.h"

void SceneManager::ChangeScene(SceneType newScene)
{
    if (m_currentScene)
        m_currentScene->Shutdown(m_registry);

    m_currentScene = m_scenes[newScene];

    if (m_currentScene)
        m_currentScene->Initialize(m_registry);
}

void SceneManager::HandleInput()
{
    if (m_currentScene)
        m_currentScene->HandleInput(m_registry);
}

void SceneManager::Update()
{
    if (m_currentScene)
        m_currentScene->Update(m_registry);
}

void SceneManager::Draw()
{
    if (m_currentScene)
        m_currentScene->Draw(m_registry);
}

void SceneManager::RegisterScene(SceneType sceneType, std::shared_ptr<IScene> scene)
{
    scene->SetSceneManager(this);
    m_scenes[sceneType] = scene;
}

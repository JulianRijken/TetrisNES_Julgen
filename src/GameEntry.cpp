#include <Camera.h>
#include <Julgen.h>
#include <Resources.h>
#include <Scene.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>

void jul::Julgen::PreInit() {}

void ExampleScene(jul::Scene& scene)
{
    // Add camrea to the scene
    auto* camera = scene.AddGameObject("Camera");
    camera->AddComponent<jul::Camera>(5, 16.0 / 9.0);

    // Add image to the screen
    auto* logo = scene.AddGameObject("Logo");
    logo->AddComponent<jul::SpriteRenderer>(jul::Resources::GetSprite("Logo"));
}

void jul::Julgen::GameStart()
{
    // Load image
    Resources::LoadSprite("Logo", "Julgen_Logo_BB_Retro.png", 32);

    // Bind scene (can be used to load multiple times)
    SceneManager::GetInstance().BindScene(0, ExampleScene);

    // Load scene
    SceneManager::GetInstance().LoadScene(0);
}

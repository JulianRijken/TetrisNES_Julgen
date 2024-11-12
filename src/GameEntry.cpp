#include <Camera.h>
#include <GameSettings.h>
#include <GameTime.h>
#include <Input.h>
#include <Julgen.h>
#include <Resources.h>
#include <Scene.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TweenEngine.h>

#include "GameState.h"
#include "Tetris.h"

using nes::Tetris;


void jul::Julgen::PreInit()
{
    jul::GameSettings::g_RenderWidth = Tetris::PIXEL_WIDTH * Tetris::WINDOW_SCALE;
    jul::GameSettings::g_RenderHeight = Tetris::PIXEL_HEIGHT * Tetris::WINDOW_SCALE;
    jul::GameSettings::g_WindowWidth = jul::GameSettings::g_RenderWidth;
    jul::GameSettings::g_WindowHeight = jul::GameSettings::g_RenderHeight;
    jul::GameSettings::g_Volume = 0.2f;

    // Uses original NES fps
    jul::GameTime::SetFixedDeltaTime(1.0 / Tetris::FRAME_RATE);

    Input::AddAction(Tetris::InputBind::RotateLeft, { { SDL_SCANCODE_Q }, {}, {} });
    Input::AddAction(Tetris::InputBind::RotateRight, { { SDL_SCANCODE_E }, {}, {} });
    Input::AddAction(Tetris::InputBind::MoveLeft, { { SDL_SCANCODE_A }, {}, {} });
    Input::AddAction(Tetris::InputBind::MoveRight, { { SDL_SCANCODE_D }, {}, {} });
    Input::AddAction(Tetris::InputBind::MoveDown, { { SDL_SCANCODE_S }, {}, {} });
}

void ExampleScene(jul::Scene& scene)
{
    auto* camera = scene.AddGameObject("Camera");
    camera->AddComponent<jul::Camera>(14, jul::GameSettings::GetAspectRatio());
    camera->GetTransform().SetLocalPosition(16, -14, 0);

    auto* background = scene.AddGameObject("Background");
    background->AddComponent<jul::SpriteRenderer>(jul::Resources::GetSprite("Background"), 100);

    auto* gameState = scene.AddGameObject("GameState");
    gameState->AddComponent<nes::GameState>();
}

void jul::Julgen::GameStart()
{
    Resources::LoadSprite("Background", "BackGround.png", 8);
    Resources::LoadSprite("Blocks", "Blocks.png", 8, { 0, 0 }, 10, 5);

    Resources::BindSound(Tetris::Sounds::Place, "SFX/SFX 8.mp3", "place");
    Resources::BindSound(Tetris::Sounds::SideMove, "SFX/SFX 4.mp3", "sideMove");
    Resources::BindSound(Tetris::Sounds::Rotate, "SFX/SFX 6.mp3", "rotate");
    Resources::BindSound(Tetris::Sounds::LineClear, "SFX/SFX 11.mp3", "lineClear");
    Resources::BindSound(Tetris::Sounds::Tetris, "SFX/SFX 10.mp3", "tetris");
    Resources::BindSound(Tetris::Sounds::LevelCear, "SFX/SFX 7.mp3", "levelClear");
    Resources::BindSound(Tetris::Sounds::GameOver, "SFX/SFX 14.mp3", "gameOver");

    SceneManager::BindScene(0, ExampleScene);
    SceneManager::LoadScene(0);
}

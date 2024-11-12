#include "Block.h"

#include <GameObject.h>
#include <GameTime.h>
#include <MathExtensions.h>
#include <Resources.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>

nes::Block::Block(jul::GameObject* parentPtr, int style, int level, int renderLayer) :
    jul::Component(parentPtr, "Block"),
    m_Style(style)
{
    // rotatePivot;
    auto& scene = GetGameObject()->GetScene();

    auto* blockPivotGo = scene.AddGameObject("BlockPivot", { 0.5f, -0.5f, 0 }, GetGameObject(), false);
    m_RotatePivotTransform = &blockPivotGo->GetTransform();

    auto* spritePointGo = scene.AddGameObject("SpritePoint", { -0.5f, 0.5f, 0 }, blockPivotGo, false);
    m_SpriteRenderer = spritePointGo->AddComponent<jul::SpriteRenderer>(
        jul::Resources::GetSprite("Blocks"), renderLayer, glm::ivec2{ style, level });
}

void nes::Block::LateUpdate() { m_RotatePivotTransform->SetWorldRotation(glm::quat{}); }

void nes::Block::SetLevel(int level) { m_SpriteRenderer->SetDrawCell({ m_Style, level }); }

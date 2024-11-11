#include "Piece.h"

#include <GameObject.h>
#include <Resources.h>
#include <Scene.h>
#include <SpriteRenderer.h>
#include <TweenEngine.h>

#include "Block.h"

nes::Piece::Piece(jul::GameObject* parentPtr, int type) :
    jul::Component(parentPtr, "Piece"),
    m_TypeIndex(type)
{
    auto& scene = GetGameObject()->GetScene();

    const glm::vec2 centerOffset = PIECES[m_TypeIndex].centerOffset;

    auto* rotatePoint = scene.AddGameObject("Piece", { centerOffset.x, centerOffset.y, 0 }, GetGameObject(), false);
    m_RotatePointTransform = &rotatePoint->GetTransform();


    for(int y = 0; y < PIECE_GRID_SIZE; ++y)
    {
        for(int x = 0; x < PIECE_GRID_SIZE; ++x)
        {
            // NOTE: Rotation index is set to 0!
            if(PIECES[m_TypeIndex].rotations[0][y][x])
            {
                auto* block =
                    scene.AddGameObject("Block", { x - centerOffset.x, -y - centerOffset.y, 0 }, rotatePoint, false);
                block->AddComponent<Block>(GetStyle());
            }
        }
    }
}

int nes::Piece::GetTypeIndex() const { return m_TypeIndex; }

int nes::Piece::GetRotationIndex() const { return m_RotationIndex; }

void nes::Piece::SetRotation(int targetRotation)
{
    const double fromRotation = m_RotationIndex * -90.0;
    const double toRotation = targetRotation * -90.0;

    const glm::quat fromQuat = glm::quat(glm::radians(glm::vec3{ 0, 0, fromRotation }));
    const glm::quat toQuat = glm::quat(glm::radians(glm::vec3{ 0, 0, toRotation }));


    jul::TweenEngine::Start(
        { .duration = 0.1,
          .easeFunction = jul::EaseFunction::SineOut,
          .onUpdate = [this, fromQuat, toQuat](double value)
          { m_RotatePointTransform->SetLocalRotation(glm::slerp(fromQuat, toQuat, static_cast<float>(value))); } },
        m_RotatePointTransform);


    // m_RotatePointTransform->SetLocalRotation(glm::vec3{ 0, 0, targetRotation * -90.0 });
    m_RotationIndex = targetRotation;
}

void nes::Piece::MoveGridPosition(glm::ivec2 moveDelta) { SetGridPosition(m_GridPosition + moveDelta); }

void nes::Piece::SetGridPosition(glm::ivec2 gridPosition, bool tween)
{
    const glm::vec3 toPosition = { gridPosition.x, gridPosition.y, 0 };

    if(tween)
    {
        const glm::vec3 fromPosition = GetTransform().GetLocalPosition();

        jul::TweenEngine::Start({ .duration = 0.05,
                                  .easeFunction = jul::EaseFunction::SineOut,
                                  .onUpdate = [this, fromPosition, toPosition](double value)
                                  { GetTransform().SetLocalPosition(glm::mix(fromPosition, toPosition, value)); } },
                                &GetTransform());
    }
    else
    {
        GetTransform().SetLocalPosition(toPosition);
    }

    m_GridPosition = gridPosition;
}

std::vector<glm::ivec2> nes::Piece::GetBlocksInGrid() const { return GetBlocksInGrid(m_RotationIndex); }

std::vector<glm::ivec2> nes::Piece::GetBlocksInGrid(int rotationIndex) const
{
    std::vector<glm::ivec2> blocks{};

    for(int y = 0; y < PIECE_GRID_SIZE; ++y)
        for(int x = 0; x < PIECE_GRID_SIZE; ++x)
            if(PIECES[m_TypeIndex].rotations[rotationIndex][y][x])
                blocks.emplace_back(x + m_GridPosition.x, -y + m_GridPosition.y);

    return blocks;
}

int nes::Piece::GetStyle() const { return PIECES[m_TypeIndex].style; }

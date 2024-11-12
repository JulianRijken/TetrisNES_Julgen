#include "Piece.h"

#include <GameObject.h>
#include <Resources.h>
#include <Scene.h>
#include <SpriteRenderer.h>
#include <TweenEngine.h>

#include "Block.h"

nes::Piece::Piece(jul::GameObject* parentPtr, int type, int level) :
    jul::Component(parentPtr, "Piece"),
    m_TypeIndex(type)
{
    auto& scene = GetGameObject()->GetScene();

    glm::vec2 centerOffset = PIECES[m_TypeIndex].centerOffset;
    centerOffset.y -= 1;

    auto* rotatePoint =
        scene.AddGameObject("rotatePoint", { centerOffset.x, centerOffset.y, 0 }, GetGameObject(), false);
    m_RotatePointTransform = &rotatePoint->GetTransform();


    // auto* debugBlockPivot = scene.AddGameObject("debugBlockPivot", { 0, 0, 0 }, GetGameObject(), false);
    // debugBlockPivot->AddComponent<Block>(3, -4);

    // auto* debugRotatePoint = scene.AddGameObject("debugRotatePoint", { 0, 0, 0 }, rotatePoint, false);
    // debugRotatePoint->AddComponent<Block>(2, 10);


    for(int y = 0; y < PIECE_GRID_SIZE; ++y)
    {
        for(int x = 0; x < PIECE_GRID_SIZE; ++x)
        {
            // auto* debugPieceGrid = scene.AddGameObject("debugPieceGrid", { x, y, 0 }, GetGameObject(), false);
            // debugPieceGrid->AddComponent<Block>(4, -10);

            // NOTE: Rotation index is set to 0!
            // NOTE: The Y is now going from 3 - 0 instead of 0 - 3 because we read the array from top left
            if(PIECES[m_TypeIndex].rotations[0][PIECE_GRID_SIZE - 1 - y][x])
            {
                // - centerOffset to undo the pivot point
                auto* block = scene.AddGameObject(
                    "Block",
                    { static_cast<float>(x) - centerOffset.x, static_cast<float>(y) - centerOffset.y, 0 },
                    rotatePoint,
                    false);
                block->AddComponent<Block>(GetStyle(), level);
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
        { .duration = INTERP_DURATION_ROTATION,
          .easeFunction = jul::EaseFunction::SineOut,
          .onUpdate = [this, fromQuat, toQuat](double value)
          { m_RotatePointTransform->SetLocalRotation(glm::slerp(fromQuat, toQuat, static_cast<float>(value))); } },
        m_RotatePointTransform);


    // m_RotatePointTransform->SetLocalRotation(glm::vec3{ 0, 0, targetRotation * -90.0 });
    m_RotationIndex = targetRotation;
}

void nes::Piece::MoveGridPosition(const glm::ivec2& moveDelta, bool tween, jul::EaseFunction::Type easeFunction)
{
    SetGridPosition(m_GridPosition + moveDelta, tween, easeFunction);
}

void nes::Piece::SetGridPosition(const glm::ivec2& gridPosition, bool tween, jul::EaseFunction::Type easeFunction)
{
    const glm::vec3 toPosition = { gridPosition.x, gridPosition.y, 0 };

    if(tween)
    {
        const glm::vec3 fromPosition = GetTransform().GetLocalPosition();

        jul::TweenEngine::Start({ .duration = INTERP_DURATION_MOVE,
                                  .easeFunction = easeFunction,
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
            // NOTE: The Y is now going from 3 - 0 instead of 0 - 3 because we read the array from top left
            if(PIECES[m_TypeIndex].rotations[rotationIndex][PIECE_GRID_SIZE - 1 - y][x])
                blocks.emplace_back(x + m_GridPosition.x, y + m_GridPosition.y);

    return blocks;
}

int nes::Piece::GetStyle() const { return PIECES[m_TypeIndex].style; }

#include "GameState.h"

#include <Component.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <Input.h>
#include <Locator.h>
#include <MathExtensions.h>
#include <SceneManager.h>
#include <Sound.h>
#include <TweenEngine.h>

#include "Grid.h"
#include "Piece.h"
#include "Tetris.h"

using namespace jul;

nes::GameState::GameState(jul::GameObject* parentPtr) :
    jul::Component(parentPtr, "GameState")
{
    jul::Input::Bind(Tetris::InputBind::RotateLeft, 0, true, this, &GameState::OnRotateLeftInput);
    jul::Input::Bind(Tetris::InputBind::RotateRight, 0, true, this, &GameState::OnRotateRightInput);
    jul::Input::Bind(Tetris::InputBind::MoveLeft, 0, true, this, &GameState::OnMoveLeftInput);
    jul::Input::Bind(Tetris::InputBind::MoveRight, 0, true, this, &GameState::OnMoveRightInput);
    jul::Input::Bind(Tetris::InputBind::MoveDown, 0, true, this, &GameState::OnMoveDownInput);
    jul::Input::Bind(Tetris::InputBind::MoveUp, 0, true, this, &GameState::OnMoveUpInput);

    // Spawn grid in scene
    m_Grid = GetGameObject()->GetScene().AddGameObject("Playfield", { 12, -24, 0 })->AddComponent<Grid>();

    SpawnNextPiece();
}

void nes::GameState::FixedUpdate()
{
    if(m_ActivePiece == nullptr)
        return;

    if(m_Grid->IsInClearingTween())
        return;

    const int framesPerDrop = Tetris::FRAMES_FOR_DROP_PER_LEVEL[std::clamp(m_CurrentLevel, 0, Tetris::MAX_LEVELS)];
    if(m_MoveFrameCount > (m_HardDrop ? Tetris::FRAMES_FOR_DROP_HARD_DROP : framesPerDrop))
    {
        TryMoveActivePiece({ 0, -1 });
        m_MoveFrameCount = 0;
    }

    m_MoveFrameCount++;


    if(CanMove({ 0, -1 }))
    {
        ResetLockFrames();
    }
    else
    {
        m_LockFrameCount++;
        if(m_LockFrameCount > Tetris::FRAMES_FOR_LOCK)
            PlaceActivePiece();
    }
}

void nes::GameState::OnRotateLeftInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryRotatePiece(RotationDirection::Left);
}

void nes::GameState::OnRotateRightInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryRotatePiece(RotationDirection::Right);
}

void nes::GameState::OnMoveLeftInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryMoveActivePiece({ -1, 0 });
}

void nes::GameState::OnMoveRightInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryMoveActivePiece({ 1, 0 });
}

void nes::GameState::OnMoveDownInput(const jul::InputContext& context)
{
    m_HardDrop = context.state == jul::ButtonState::Down;


    if(context.state != jul::ButtonState::Down)
        return;

    TryMoveActivePiece({ 0, -1 });
}

void nes::GameState::OnMoveUpInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    PlaceActivePieceMovedDown();
}

bool nes::GameState::TryRotatePiece(RotationDirection direction)
{
    const int fromR{ m_ActivePiece->GetRotationIndex() };
    const int toR = jul::math::ClampLoop(
        m_ActivePiece->GetRotationIndex() + static_cast<int>(direction), 0, Piece::PIECE_ROTATION_COUNT - 1);

    int checkSet{ 0 };
    const int checkType{ m_ActivePiece->GetTypeIndex() == 0 ? 0 : 1 };

    if(checkType == 0)
    {
        if((fromR == 0 and toR == 1) or (fromR == 3 and toR == 2))
            checkSet = 0;
        else if((fromR == 1 and toR == 0) or (fromR == 2 and toR == 3))
            checkSet = 1;
        else if((fromR == 1 and toR == 2) or (fromR == 0 and toR == 3))
            checkSet = 2;
        else if((fromR == 2 and toR == 1) or (fromR == 3 and toR == 0))
            checkSet = 3;
    }
    else  // I Piece Sets// J L T S Z Piece Sets
    {
        if((fromR == 0 and toR == 1) or (fromR == 2 and toR == 1))
            checkSet = 0;
        else if((fromR == 1 and toR == 0) or (fromR == 1 and toR == 2))
            checkSet = 1;
        else if((fromR == 2 and toR == 3) or (fromR == 0 and toR == 3))
            checkSet = 2;
        else if((fromR == 3 and toR == 2) or (fromR == 3 and toR == 0))
            checkSet = 3;
    }


    for(const glm::ivec2& kick : Piece::KICK_CHECKS[checkType][checkSet])
    {
        if(CanMove(kick, toR))
        {
            Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::Rotate);
            m_ActivePiece->SetRotation(toR);
            m_ActivePiece->MoveGridPosition(kick);
            ResetLockFrames();
            return true;
        }
    }

    return false;
}

bool nes::GameState::CanMove(const glm::ivec2& moveDelta, int customRotation)
{
    assert(m_ActivePiece);

    // Call optinally with custom rotation
    // Is used for the kick checks and seeing if next rotation works
    const std::vector<glm::ivec2>& blocksInGrid =
        customRotation == -1 ? m_ActivePiece->GetBlocksInGrid() : m_ActivePiece->GetBlocksInGrid(customRotation);

    for(const glm::ivec2& blockPosition : blocksInGrid)
    {
        const glm::ivec2 targetPosition{ blockPosition.x + moveDelta.x, blockPosition.y + moveDelta.y };

        if(targetPosition.x < 0 or targetPosition.x >= Tetris::GRID_SIZE_X)
            return false;

        // If target position is under map
        if(targetPosition.y < 0)
            return false;

        if(m_Grid->IsBlockOverlapping(targetPosition))
            return false;
    }

    return true;
}

void nes::GameState::PlaceActivePiece()
{
    assert(m_ActivePiece);

    for(const auto& blockPosition : m_ActivePiece->GetBlocksInGrid())
    {
        if(blockPosition.y >= Tetris::GRID_SIZE_Y)
        {
            EndGame();
            return;
        }

        m_Grid->AddBlockToGrid({ blockPosition.x, blockPosition.y }, m_ActivePiece->GetStyle(), m_CurrentLevel);
    }

    m_Statistics[m_ActivePiece->GetTypeIndex()]++;
    Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::Place);

    const int rowsCleared = m_Grid->TryClearRows();
    if(rowsCleared > 0)
    {
        m_LinesCleared += rowsCleared;
        m_Score += Tetris::SCORE_PER_ROW[rowsCleared - 1];
        UpdateCurrentLevel();
    }

    SpawnNextPiece();
}

bool nes::GameState::TryMoveActivePiece(const glm::ivec2& moveDelta)
{
    if(m_ActivePiece == nullptr)
        return false;

    if(CanMove(moveDelta))
    {
        if(moveDelta.x != 0)
            Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::SideMove);

        m_ActivePiece->MoveGridPosition(moveDelta);
        return true;
    }

    return false;
}

void nes::GameState::SpawnNextPiece()
{
    if(m_ActivePiece != nullptr)
        m_ActivePiece->GetGameObject()->Destroy();

    auto& scene = GetGameObject()->GetScene();
    auto* pieceGo = scene.AddGameObject("Piece", {}, m_Grid->GetGameObject(), false);
    m_ActivePiece = pieceGo->AddComponent<nes::Piece>(jul::math::RandomRange(0, 6), m_CurrentLevel);
    m_ActivePiece->SetGridPosition({ Tetris::GRID_SIZE_X / 2 - Piece::PIECE_GRID_SIZE / 2, Tetris::GRID_SIZE_Y },
                                   false);
}

void nes::GameState::PlaceActivePieceMovedDown()
{
    int moveDistance{ -1 };
    while(CanMove({ 0, moveDistance }))
        moveDistance--;

    TryMoveActivePiece({ 0, moveDistance + 1 });
    PlaceActivePiece();
}

void nes::GameState::EndGame()
{
    //
    jul::SceneManager::GetInstance().LoadScene(0);
}

void nes::GameState::ResetLockFrames() { m_LockFrameCount = 0; }

void nes::GameState::UpdateCurrentLevel()
{
    const int newLevel{ m_LinesCleared / 10 };

    if(newLevel > m_CurrentLevel)
    {
        jul::Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::LevelCear);
        m_CurrentLevel = newLevel;
    }
}

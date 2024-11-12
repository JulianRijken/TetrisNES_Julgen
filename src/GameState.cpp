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

#include "Block.h"
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

    auto& scene = GetGameObject()->GetScene();
    m_Playfield = scene.AddGameObject("Playfield", { 12, -24, 0 });


    // for(int y = 0; y < Tetris::GRID_SIZE_Y; ++y)
    // {
    //     for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
    //     {
    //         auto* blockGo = GetGameObject()->GetScene().AddGameObject("Block", { x, y, -10 }, m_Playfield, false);
    //         blockGo->AddComponent<nes::Block>(4);
    //     }
    // }


    SpawnNextPiece();
}


void nes::GameState::FixedUpdate()
{
    if(m_ActivePiece == nullptr)
        return;

    if(m_ClearingRows)
        return;

    m_FrameCount++;

    if(m_FrameCount > Tetris::GAME_FRAMES_PER_CELL_PER_LEVEL[std::clamp(m_CurrentLevel, 0, Tetris::MAX_LEVELS)] or
       m_HardDrop)
    {
        m_FrameCount = 0;

        TryMoveActivePiece({ 0, -1 });
    }

    if(CanMove({ 0, -1 }))
    {
        m_LockFrameCount = 0;
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
    else // I Piece Sets// J L T S Z Piece Sets 
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
    const auto& blocksInGrid =
        customRotation == -1 ? m_ActivePiece->GetBlocksInGrid() : m_ActivePiece->GetBlocksInGrid(customRotation);

    for(const auto& blockPosition : blocksInGrid)
    {
        const glm::ivec2 targetPosition{ blockPosition.x + moveDelta.x, blockPosition.y + moveDelta.y };

        if(targetPosition.x < 0 or targetPosition.x >= Tetris::GRID_SIZE_X)
            return false;

        // If target position is under map
        if(targetPosition.y < 0)
            return false;


        for(int y = 0; y < Tetris::GRID_SIZE_Y; ++y)
        {
            for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
            {
                // Igunore empty spaces
                if(m_Blocks[x][y] == nullptr)
                    continue;

                // If target position is at a block
                if(targetPosition == glm::ivec2{ x, y })
                    return false;
            }
        }
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

        Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::Place);
        AddBlockToGrid({ blockPosition.x, blockPosition.y }, m_ActivePiece->GetStyle());
    }

    SpawnNextPiece();
    ClearRows();
}

bool nes::GameState::TryMoveActivePiece(const glm::ivec2& moveDelta)
{
    if(m_ActivePiece == nullptr)
        return false;

    if(CanMove(moveDelta))
    {
        m_ActivePiece->MoveGridPosition(moveDelta);

        if(moveDelta.x != 0)
            Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::SideMove);

        return true;
    }

    return false;
}

void nes::GameState::SpawnNextPiece()
{
    if(m_ActivePiece != nullptr)
        m_ActivePiece->GetGameObject()->Destroy();

    auto& scene = GetGameObject()->GetScene();
    auto* pieceGo = scene.AddGameObject("Piece", {}, m_Playfield, false);
    m_ActivePiece = pieceGo->AddComponent<nes::Piece>(jul::math::RandomRange(0, 6));
    m_ActivePiece->SetGridPosition(
        { Tetris::GRID_SIZE_X / 2 - Piece::PIECE_GRID_SIZE / 2, Tetris::GRID_SIZE_Y + Piece::PIECE_GRID_SIZE }, false);
}

void nes::GameState::ClearRows()
{
    std::vector<int> rowsToClear{};

    // NOTE: We find rows from top to bottom as this is the order they get cleared
    for(int y = Tetris::GRID_SIZE_Y - 1; y >= 0; --y)
    {
        int blocksInRow{};

        for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
            if(m_Blocks[x][y] != nullptr)  // If block exists
                blocksInRow++;

        if(blocksInRow == Tetris::GRID_SIZE_X)
            rowsToClear.emplace_back(y);
    }

    if(rowsToClear.empty())
        return;

    m_ClearingRows = true;


    for(const int rowToClear : rowsToClear)
    {
        for(int colToClear = 0; colToClear < Tetris::GRID_SIZE_X; colToClear++)
        {

            const int halfSize = Tetris::GRID_SIZE_X / 2;
            double delay = 0;
            if(colToClear < halfSize)
                delay = halfSize - colToClear;
            else
                delay = colToClear - (halfSize - 1);

            delay /= halfSize;

            jul::TweenEngine::Start(
                {
                    // .from = 1.0,
                    // .to = 0.0,
                    .duration = delay * ROW_CLEAR_DURATION,
                    // .onUpdate = [blockGo](double value) { blockGo->GetTransform().SetLocalScale(value, value, value);
                    // },
                    .onEnd =
                        [this, colToClear, rowToClear]()
                    {
                        assert(m_Blocks[colToClear][rowToClear]);

                        m_Blocks[colToClear][rowToClear]->GetGameObject()->Destroy();
                        m_Blocks[colToClear][rowToClear] = nullptr;
                    },
                },
                this);
        }
    }

    auto moveRowsDown = [this, rowsToClear]()
    {
        std::unordered_map<Block*, int> fallenBlocks{};

        // For each row
        for(const int row : rowsToClear)
        {
            // All rows above
            for(int y = row; y < Tetris::GRID_SIZE_Y - 1; ++y)
            {
                for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
                {
                    Block* blockToMove = m_Blocks[x][y + 1];

                    // One above
                    if(blockToMove != nullptr)
                    {
                        // Row should be cleared!
                        assert(m_Blocks[x][y] == nullptr);

                        m_Blocks[x][y] = blockToMove;
                        m_Blocks[x][y + 1] = nullptr;

                        if(fallenBlocks.contains(blockToMove))
                            fallenBlocks[blockToMove]++;
                        else
                            fallenBlocks[blockToMove] = 1;
                    }
                }
            }
        }

        for(const auto& [block, distance] : fallenBlocks)
        {
            const glm::vec3 fromPosition = block->GetTransform().GetLocalPosition();
            const glm::vec3 toPosition = fromPosition - glm::vec3{ 0, distance, 0 };

            jul::TweenEngine::Start(
                {
                    .duration = ROW_FALL_SPEED * distance - jul::math::RandomRange(0.1, 0.2),
                    .easeFunction = jul::EaseFunction::BounceOut,
                    .onUpdate =
                        [block, fromPosition, toPosition](double value)
                    {
                        const glm::vec3 targetPosition = glm::mix(fromPosition, toPosition, value);
                        block->GetTransform().SetLocalPosition(targetPosition);
                    },
                },
                block);
        }

        jul::TweenEngine::Start(
            {
                .delay = ROW_FALL_SPEED * rowsToClear.size(),
                .duration = 0,
                .onEnd = [this]() { m_ClearingRows = false; },
            },
            this);
    };


    // Wait for rows to dissipair
    jul::TweenEngine::Start(
        {
            .delay = ROW_CLEAR_DURATION,
            .duration = 0,
            .onEnd = moveRowsDown,
        },
        this);
}

void nes::GameState::AddBlockToGrid(const glm::ivec2& gridPosition, int style)
{
    auto* blockGo =
        GetGameObject()->GetScene().AddGameObject("Block", { gridPosition.x, gridPosition.y, 0 }, m_Playfield, false);
    m_Blocks[gridPosition.x][gridPosition.y] = blockGo->AddComponent<nes::Block>(style);
}

void nes::GameState::EndGame() { jul::SceneManager::GetInstance().LoadScene(0); }

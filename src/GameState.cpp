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
    m_Playfield = scene.AddGameObject("Playfield", { 12, -5, 0 });


    // AddBlockToGrid({ 0, -19 });
    // AddBlockToGrid({ 1, -19 });
    // AddBlockToGrid({ 2, -19 });
    // AddBlockToGrid({ 3, -19 });
    // AddBlockToGrid({ 4, -19 });
    // AddBlockToGrid({ 5, -19 });
    // AddBlockToGrid({ 6, -19 });
    // AddBlockToGrid({ 7, -19 });
    // AddBlockToGrid({ 8, -19 });
    // AddBlockToGrid({ 9, -19 });

    // AddBlockToGrid({ 0, -18 });
    // AddBlockToGrid({ 1, -18 });
    // AddBlockToGrid({ 2, -18 });


    // AddBlockToGrid({ 5, -18 });
    // AddBlockToGrid({ 6, -18 });
    // AddBlockToGrid({ 7, -18 });
    // AddBlockToGrid({ 8, -18 });
    // AddBlockToGrid({ 9, -18 });

    // AddBlockToGrid({ 0, -17 });
    // AddBlockToGrid({ 1, -17 });
    // AddBlockToGrid({ 2, -17 });
    // AddBlockToGrid({ 3, -17 });
    // AddBlockToGrid({ 4, -17 });
    // AddBlockToGrid({ 5, -17 });
    // AddBlockToGrid({ 6, -17 });
    // AddBlockToGrid({ 7, -17 });
    // AddBlockToGrid({ 8, -17 });
    // AddBlockToGrid({ 9, -17 });

    // AddBlockToGrid({ 0, -16 });
    // AddBlockToGrid({ 1, -16 });
    // AddBlockToGrid({ 2, -16 });


    // AddBlockToGrid({ 5, -16 });
    // AddBlockToGrid({ 6, -16 });
    // AddBlockToGrid({ 7, -16 });
    // AddBlockToGrid({ 8, -16 });
    // AddBlockToGrid({ 9, -16 });

    // AddBlockToGrid({ 0, -15 });
    // AddBlockToGrid({ 1, -15 });
    // AddBlockToGrid({ 2, -15 });
    // AddBlockToGrid({ 3, -15 });
    // AddBlockToGrid({ 4, -15 });
    // AddBlockToGrid({ 5, -15 });
    // AddBlockToGrid({ 6, -15 });
    // AddBlockToGrid({ 7, -15 });
    // AddBlockToGrid({ 8, -15 });
    // AddBlockToGrid({ 9, -15 });


    // AddBlockToGrid({ 6, -14 });
    // AddBlockToGrid({ 7, -14 });
    // AddBlockToGrid({ 8, -14 });
    // AddBlockToGrid({ 9, -14 });

    // AddBlockToGrid({ 7, -13 });
    // AddBlockToGrid({ 8, -13 });
    // AddBlockToGrid({ 9, -13 });

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

        TryMovePiece({ 0, -1 });
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

    RotatePiece(RotationDirection::Left);
}

void nes::GameState::OnRotateRightInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    RotatePiece(RotationDirection::Right);
}

void nes::GameState::OnMoveLeftInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryMovePiece({ -1, 0 });
}

void nes::GameState::OnMoveRightInput(const jul::InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    TryMovePiece({ 1, 0 });
}

void nes::GameState::OnMoveDownInput(const jul::InputContext& context)
{
    m_HardDrop = context.state == jul::ButtonState::Down;


    if(context.state != jul::ButtonState::Down)
        return;

    TryMovePiece({ 0, -1 });
}

void nes::GameState::RotatePiece(RotationDirection direction)
{
    const int fromR{ m_ActivePiece->GetRotationIndex() };
    const int toR = jul::math::ClampLoop(
        m_ActivePiece->GetRotationIndex() + static_cast<int>(direction), 0, Piece::PIECE_ROTATION_COUNT - 1);

    int checkSet{ 0 };
    const int checkType{ m_ActivePiece->GetTypeIndex() == 0 ? 0 : 1 };
    // clang-format off
    if (checkType == 0) 
    {
             if ((fromR == 0 and toR == 1) or (fromR == 3 and toR == 2))    checkSet = 0;
        else if ((fromR == 1 and toR == 0) or (fromR == 2 and toR == 3))    checkSet = 1;
        else if ((fromR == 1 and toR == 2) or (fromR == 0 and toR == 3))    checkSet = 2;
        else if ((fromR == 2 and toR == 1) or (fromR == 3 and toR == 0))    checkSet = 3;
    }
    else // I Piece Sets// J L T S Z Piece Sets 
    {
             if ((fromR == 0 and toR == 1) or (fromR == 2 and toR == 1))	checkSet = 0;
        else if ((fromR == 1 and toR == 0) or (fromR == 1 and toR == 2))	checkSet = 1;
        else if ((fromR == 2 and toR == 3) or (fromR == 0 and toR == 3))	checkSet = 2;
        else if ((fromR == 3 and toR == 2) or (fromR == 3 and toR == 0))	checkSet = 3;
    }
    // clang-format on


    for(const glm::ivec2& kick : Piece::KICK_CHECKS[checkType][checkSet])
    {
        if(CanMove(kick, toR))
        {
            Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::Rotate);
            m_ActivePiece->SetRotation(toR);
            m_ActivePiece->MoveGridPosition(kick);
            return;
        }
    }

    m_ActivePiece->SetRotation(fromR);
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

        if(targetPosition.y <= -Tetris::GRID_SIZE_Y)
            return false;


        for(int y = 0; y < Tetris::GRID_SIZE_Y; ++y)
        {
            for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
            {
                if(m_Blocks[x][y] == nullptr)
                    continue;

                if(targetPosition == glm::ivec2{ x, -y })
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
        if(blockPosition.y > 0)
        {
            EndGame();
            return;
        }

        Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::Place);
        AddBlockToGrid({ blockPosition.x, blockPosition.y }, m_ActivePiece->GetStyle());
    }

    SpawnNextPiece();
    TryClearRows();
}

void nes::GameState::TryMovePiece(const glm::ivec2& moveDelta)
{
    if(m_ActivePiece == nullptr)
        return;

    if(CanMove(moveDelta))
    {
        m_ActivePiece->MoveGridPosition(moveDelta);

        if(moveDelta.x != 0)
            Locator::Get<Sound>().PlaySound((int)Tetris::Sounds::SideMove);
    }
}

void nes::GameState::SpawnNextPiece()
{
    if(m_ActivePiece != nullptr)
        m_ActivePiece->GetGameObject()->Destroy();

    auto& scene = GetGameObject()->GetScene();
    auto* pieceGo = scene.AddGameObject("Piece", {}, m_Playfield, false);
    m_ActivePiece = pieceGo->AddComponent<nes::Piece>(jul::math::RandomRange(0, 6));
    m_ActivePiece->SetGridPosition({ Tetris::GRID_SIZE_X / 2 - Piece::PIECE_GRID_SIZE / 2, Piece::PIECE_GRID_SIZE },
                                   false);
}

void nes::GameState::TryClearRows()
{
    std::vector<int> rowsToClear{};

    for(int y = 0; y < Tetris::GRID_SIZE_Y; ++y)
    {
        int blocksInRow{};

        for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
            if(m_Blocks[x][y] != nullptr)
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

        for(const int row : rowsToClear)
        {
            for(int y = row; y > 1; --y)
            {
                for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
                {
                    Block* blockToMove = m_Blocks[x][y - 1];

                    // One above
                    if(blockToMove != nullptr)
                    {
                        // Row should be cleared!
                        assert(m_Blocks[x][y] == nullptr);

                        m_Blocks[x][y] = blockToMove;
                        m_Blocks[x][y - 1] = nullptr;

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


    // // Wait for rows to dissipair
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
    m_Blocks[gridPosition.x][-gridPosition.y] = blockGo->AddComponent<nes::Block>(style);
}

void nes::GameState::EndGame() { jul::SceneManager::GetInstance().LoadScene(0); }
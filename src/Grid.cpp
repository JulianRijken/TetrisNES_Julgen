#include "Grid.h"

#include <GameObject.h>
#include <MathExtensions.h>
#include <SceneManager.h>
#include <TweenEngine.h>

#include "Block.h"

nes::Grid::Grid(jul::GameObject* parentPtr) :
    jul::Component(parentPtr, "Grid")
{
}

int nes::Grid::TryClearRows()
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
        return 0;

    const int rowCount = static_cast<int>(rowsToClear.size());
    assert(rowCount <= 4);


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
                    .duration = delay * ROW_CLEAR_DURATION,
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

    return rowCount;
}

bool nes::Grid::IsInClearingTween() const { return m_ClearingRows; }

void nes::Grid::AddBlockToGrid(const glm::ivec2& gridPosition, int style, int level)
{

    assert(IsBlockInGrid(gridPosition) && "Can't add block outside of grid");

    auto* blockGo = GetGameObject()->GetScene().AddGameObject(
        "Block", { gridPosition.x, gridPosition.y, 0 }, GetGameObject(), false);
    m_Blocks[gridPosition.x][gridPosition.y] = blockGo->AddComponent<nes::Block>(style, level);
}

bool nes::Grid::IsBlockOverlapping(const glm::ivec2& block) const
{
    for(int y = 0; y < Tetris::GRID_SIZE_Y; ++y)
    {
        for(int x = 0; x < Tetris::GRID_SIZE_X; ++x)
        {
            if(m_Blocks[x][y] == nullptr)
                continue;

            if(block == glm::ivec2{ x, y })
                return true;
        }
    }

    return false;
}

bool nes::Grid::IsBlockInGrid(const glm::ivec2& block) const
{
    return (block.x >= 0 and block.x < Tetris::GRID_SIZE_X and block.y >= 0 and block.y < Tetris::GRID_SIZE_Y);
}

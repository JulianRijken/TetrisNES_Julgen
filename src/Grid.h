#ifndef GRID_H
#define GRID_H

#include <Component.h>
#include <Tetris.h>

#include <glm/vec2.hpp>

namespace nes
{

    class Block;

    class Grid final : public jul::Component
    {
    public:
        Grid(jul::GameObject* parentPtr);

        void AddBlockToGrid(const glm::ivec2& gridPosition, int style, int level);

        int TryClearRows();
        bool IsInClearingTween() const;
        bool IsBlockOverlapping(const glm::ivec2& block) const;

    private:
        inline static constexpr float ROW_CLEAR_DURATION{ 0.5f };
        inline static constexpr float ROW_FALL_SPEED{ 0.5f };

        bool IsBlockInGrid(const glm::ivec2& block) const;

        bool m_ClearingRows{};
        Block* m_Blocks[Tetris::GRID_SIZE_X][Tetris::GRID_SIZE_Y]{};
    };
}  // namespace nes

#endif  // GRID_H

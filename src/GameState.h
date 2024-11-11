#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Component.h>
#include <Tetris.h>

#include <glm/vec2.hpp>

namespace jul
{
    class InputContext;
}

namespace nes
{

    class Block;
    class Piece;

    class GameState final : public jul::Component, public jul::IEventListener
    {
    public:
        GameState(jul::GameObject* parentPtr);

    private:
        enum class RotationDirection
        {
            Left = -1,
            Right = 1
        };

        inline static constexpr float ROW_CLEAR_DURATION{ 0.5f };
        inline static constexpr float ROW_FALL_SPEED{ 0.5f };


        void FixedUpdate() override;

        void OnRotateLeftInput(const jul::InputContext& context);
        void OnRotateRightInput(const jul::InputContext& context);
        void OnMoveLeftInput(const jul::InputContext& context);
        void OnMoveRightInput(const jul::InputContext& context);
        void OnMoveDownInput(const jul::InputContext& context);

        void RotatePiece(RotationDirection direction);
        bool CanMove(const glm::ivec2& moveDelta, int customRotation = -1);
        void PlaceActivePiece();


        void TryMovePiece(const glm::ivec2& moveDelta);
        void SpawnNextPiece();
        void TryClearRows();
        void MoveRowsDown();
        void AddBlockToGrid(const glm::ivec2& gridPosition, int style);

        void EndGame();

        int m_FrameCount{};
        int m_LockFrameCount{};
        int m_CurrentLevel{};
        bool m_HardDrop{};
        bool m_ClearingRows{};
        Piece* m_ActivePiece{};
        Block* m_Blocks[Tetris::GRID_SIZE_X][Tetris::GRID_SIZE_Y]{};
        jul::GameObject* m_Playfield{ nullptr };
    };
}  // namespace nes

#endif  // GAMESTATE_H

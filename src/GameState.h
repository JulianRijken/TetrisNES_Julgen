#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Component.h>
#include <Piece.h>
#include <Tetris.h>

#include <glm/vec2.hpp>

namespace jul
{
    struct InputContext;
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
        void OnMoveUpInput(const jul::InputContext& context);

        bool CanMove(const glm::ivec2& moveDelta, int customRotation = -1);

        void PlaceActivePiece();
        void SpawnNextPiece();
        void PlaceActivePieceMovedDown();

        bool TryRotatePiece(RotationDirection direction);
        bool TryMoveActivePiece(const glm::ivec2& moveDelta);

        void ClearRows();
        void AddBlockToGrid(const glm::ivec2& gridPosition, int style);
        void EndGame();
        void ResetLockFrames();
        void UpdateCurrentLevel();

        int m_MoveFrameCount{};
        int m_LockFrameCount{};

        int m_CurrentLevel{};
        int m_Statistics[Piece::AMOUNT_OF_TYPES_OF_PIECES]{};
        int m_Score{};
        int m_LinesCleared{};

        bool m_HardDrop{};
        bool m_ClearingRows{};
        Piece* m_ActivePiece{};
        Block* m_Blocks[Tetris::GRID_SIZE_X][Tetris::GRID_SIZE_Y]{};
        jul::GameObject* m_Playfield{ nullptr };
    };
}  // namespace nes

#endif  // GAMESTATE_H

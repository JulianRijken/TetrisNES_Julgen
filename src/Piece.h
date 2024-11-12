#ifndef PIECE_H
#define PIECE_H

#include <Component.h>
#include <EaseFunction.h>

#include <glm/vec2.hpp>

namespace nes
{
    class Piece final : public jul::Component
    {
    public:
        constexpr static int PIECE_ROTATION_COUNT{ 4 };
        constexpr static int PIECE_GRID_SIZE{ 4 };

        constexpr static int AMOUNT_OF_TYPES_OF_PIECES{ 7 };

        constexpr static float INTERP_DURATION_MOVE{ 0.08f };
        constexpr static float INTERP_DURATION_ROTATION{ 0.13f };


        constexpr static int KICK_CHECK_TYPES{ 2 };
        constexpr static int KICK_CHECK_SETS{ 4 };
        constexpr static int KICK_CHECK_TESTS{ 5 };

        struct PieceSettings
        {
            int style{};
            glm::vec2 centerOffset{};  // Only used for centering piece in the preview
            bool rotations[PIECE_ROTATION_COUNT][PIECE_GRID_SIZE][PIECE_GRID_SIZE]{};
        };

        constexpr const static PieceSettings PIECES[AMOUNT_OF_TYPES_OF_PIECES]{
            PieceSettings  // I
            { 0,
                   { 2.0f, 2.0f },  // Rotation offset for first piece from bottom left
              { {
                    { 0, 0, 0, 0 },
                    { 1, 1, 1, 1 },
                    { 0, 0, 0, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 1, 0 },
                    { 0, 0, 1, 0 },
                    { 0, 0, 1, 0 },
                    { 0, 0, 1, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 0, 0, 0 },
                    { 1, 1, 1, 1 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                } } },
            PieceSettings  // J
            { 2,
                   { 1.5f, 1.5f },  // Rotation offset for first piece from bottom left
              { {
                    { 0, 0, 0, 0 },
                    { 1, 0, 0, 0 },
                    { 1, 1, 1, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 0, 0, 0 },
                    { 1, 1, 1, 0 },
                    { 0, 0, 1, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                    { 1, 1, 0, 0 },
                } } },
            PieceSettings  // L
            {
                   1, { 1.5f, 1.5f },  // Rotation offset for first piece from bottom left
                { {
                      { 0, 0, 0, 0 },
                      { 0, 0, 1, 0 },
                      { 1, 1, 1, 0 },
                      { 0, 0, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 0, 1, 1, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 0, 0, 0 },
                      { 1, 1, 1, 0 },
                      { 1, 0, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 1, 1, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 0, 1, 0, 0 },
                  } },
                   },
            PieceSettings  // O
            { 0,
                   { 2.0f, 2.0f },  // Rotation offset for first piece from bottom left
              { {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 0, 0, 0 },
                } } },
            PieceSettings  // S
            { 2,
                   { 1.5f, 1.5f },  // Rotation offset for first piece from bottom left
              { {
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 1, 1, 0, 0 },
                    { 0, 0, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 1, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 0, 0, 1, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 0, 0, 0, 0 },
                    { 0, 1, 1, 0 },
                    { 1, 1, 0, 0 },
                },
                {
                    { 0, 0, 0, 0 },
                    { 1, 0, 0, 0 },
                    { 1, 1, 0, 0 },
                    { 0, 1, 0, 0 },
                } } },
            PieceSettings  // T
            {
                   0, { 1.5f, 1.5f },  // Rotation offset for first piece from bottom left
                { {
                      { 0, 0, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 1, 1, 1, 0 },
                      { 0, 0, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 0, 1, 1, 0 },
                      { 0, 1, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 0, 0, 0 },
                      { 1, 1, 1, 0 },
                      { 0, 1, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 1, 1, 0, 0 },
                      { 0, 1, 0, 0 },
                  } },
                   },
            PieceSettings  // Z
            {
                   1, { 1.5f, 1.5f },  // Rotation offset for first piece from bottom left
                { {
                      { 0, 0, 0, 0 },
                      { 1, 1, 0, 0 },
                      { 0, 1, 1, 0 },
                      { 0, 0, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 0, 1, 0 },
                      { 0, 1, 1, 0 },
                      { 0, 1, 0, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 0, 0, 0 },
                      { 1, 1, 0, 0 },
                      { 0, 1, 1, 0 },
                  },
                  {
                      { 0, 0, 0, 0 },
                      { 0, 1, 0, 0 },
                      { 1, 1, 0, 0 },
                      { 1, 0, 0, 0 },
                  } },
                   },
        };

        // TODO: Make sure kicks are the correct y as the playfield now goes down in y
        // clang-format off
        constexpr const static glm::ivec2 KICK_CHECKS[KICK_CHECK_TYPES][KICK_CHECK_SETS][KICK_CHECK_TESTS]
        {
            { // Checks for I
                {{  0 ,  0 },{ -2 ,  0 },{  1 ,  0 },{ -2 , -1 },{  1 ,  2 },},
                {{  0 ,  0 },{  2 ,  0 },{ -1 ,  0 },{  2 ,  1 },{ -1 , -2 },},
                {{  0 ,  0 },{ -1 ,  0 },{  2 ,  0 },{ -1 ,  2 },{  2 , -1 },},
                {{  0 ,  0 },{  1 ,  0 },{ -2 ,  0 },{  1 , -2 },{ -2 ,  1 },},
            },
            { // Checks for J L T S Z
                {{  0 ,  0 },{ -1 ,  0 },{ -1 ,  1 },{  0 , -2 },{ -1 , -2 },},
                {{  0 ,  0 },{  1 ,  0 },{  1 , -1 },{  0 ,  2 },{  1 ,  2 },},
                {{  0 ,  0 },{  1 ,  0 },{  1 ,  1 },{  0 , -2 },{  1 , -2 },},
                {{  0 ,  0 },{ -1 ,  0 },{ -1 , -1 },{  0 ,  2 },{ -1 ,  2 },},
            }      
        };
        // clang-format on

        Piece(jul::GameObject* parentPtr, int type, int level);

        [[nodiscard]] int GetTypeIndex() const;
        [[nodiscard]] int GetStyle() const;
        [[nodiscard]] int GetRotationIndex() const;

        void SetRotation(int rotation);
        void MoveGridPosition(const glm::ivec2& moveDelta, bool tween = true,
                              jul::EaseFunction::Type easeFunction = jul::EaseFunction::SineOut);
        void SetGridPosition(const glm::ivec2& gridPosition, bool tween = true,
                             jul::EaseFunction::Type easeFunction = jul::EaseFunction::SineOut);

        [[nodiscard]] std::vector<glm::ivec2> GetBlocksInGrid() const;
        [[nodiscard]] std::vector<glm::ivec2> GetBlocksInGrid(int rotation) const;

    private:
        int m_TypeIndex = 0;
        int m_RotationIndex = 0;
        glm::ivec2 m_GridPosition = { 0, 0 };
        jul::Transform* m_RotatePointTransform = nullptr;
    };

}  // namespace nes
#endif

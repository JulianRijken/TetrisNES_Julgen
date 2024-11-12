#ifndef GAME_H
#define GAME_H

namespace nes
{
    class Tetris final
    {
    public:
        inline static constexpr int GRID_SIZE_X{ 10 };
        inline static constexpr int GRID_SIZE_Y{ 20 };

        inline static constexpr int WINDOW_SCALE{ 4 };
        inline static constexpr int PIXEL_WIDTH{ 256 };
        inline static constexpr int PIXEL_HEIGHT{ 224 };
        inline static constexpr int NUMBER_SIZE{ 7 * WINDOW_SCALE };

        inline static constexpr int PLAY_FIELD_OFFSET_X{ 96 * WINDOW_SCALE };
        inline static constexpr int PLAY_FIELD_OFFSET_Y{ 25 * WINDOW_SCALE };

        inline static constexpr float CELL_SIZE{ 8 * WINDOW_SCALE };

        inline static constexpr int MAX_LINES{ 4 };
        inline static constexpr int SCORE_PER_ROW[MAX_LINES]{ 40, 100, 300, 1200 };

        inline static constexpr int MAX_LEVELS_SPRITES{ 10 };
        inline static constexpr int MAX_LEVELS{ 30 };

        inline static constexpr double FRAME_RATE{ 60.0988 };
        inline static constexpr int FRAMES_FOR_LOCK{ 40 };

        inline static constexpr int FRAMES_FOR_DROP_HARD_DROP{ 1 };
        inline static constexpr int FRAMES_FOR_DROP_PER_LEVEL[MAX_LEVELS]{
            48,  // 0
            43,  // 1
            38,  // 2
            33,  // 3
            28,  // 4
            23,  // 5
            18,  // 6
            13,  // 7
            8,   // 8
            9,   // 9

            5,  // 10
            5,  // 11
            5,  // 12

            4,  // 13
            4,  // 14
            4,  // 15

            3,  // 16
            3,  // 17
            3,  // 18

            2,  // 19
            2,  // 20
            2,  // 21
            2,  // 22
            2,  // 23
            2,  // 24
            2,  // 25
            2,  // 26
            2,  // 27
            2,  // 28

            1,  // 29
        };

        enum class InputBind
        {
            RotateLeft,
            RotateRight,
            MoveLeft,
            MoveRight,
            MoveDown,
            MoveUp
        };

        enum class Sounds
        {
            SideMove,
            Rotate,
            GameOver,
            Place,
            Tetris,
            LineClear,
            LevelCear,
        };
    };

}  // namespace nes
#endif  // GAME_H

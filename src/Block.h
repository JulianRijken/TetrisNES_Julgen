#ifndef BLOCK_H
#define BLOCK_H

#include <Component.h>


namespace jul
{
    class SpriteRenderer;
}

namespace nes
{
    class Block final : public jul::Component
    {
    public:
        Block(jul::GameObject* parentPtr, int style, int level, int renderLayer = 0);

        void LateUpdate() override;
        void SetLevel(int level);

    private:
        int m_Style = 0;
        jul::Transform* m_RotatePivotTransform = nullptr;
        jul::SpriteRenderer* m_SpriteRenderer = nullptr;
    };
}  // namespace nes


#endif  // BLOCK_H

#ifndef BLOCK_H
#define BLOCK_H

#include <Component.h>

#include <glm/glm.hpp>

namespace jul
{
    class SpriteRenderer;
}

namespace nes
{
    class Block final : public jul::Component
    {
    public:
        Block(jul::GameObject* parentPtr, int style, int renderLayer = 0);

        void LateUpdate() override;

    private:
        int m_Style = 0;
        jul::Transform* m_RotatePivotTransform = nullptr;
        jul::SpriteRenderer* m_SpriteRenderer = nullptr;
    };
}  // namespace nes


#endif  // BLOCK_H

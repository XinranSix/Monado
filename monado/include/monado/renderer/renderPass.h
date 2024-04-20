#pragma once

#include "monado/core/base.h"

#include "framebuffer.h"

namespace Monado {

    struct RenderPassSpecification {
        Ref<Framebuffer> TargetFramebuffer;
        std::string DebugName;
    };

    class RenderPass : public RefCounted {
    public:
        virtual ~RenderPass() = default;

        virtual RenderPassSpecification &GetSpecification() = 0;
        virtual const RenderPassSpecification &GetSpecification() const = 0;

        static Ref<RenderPass> Create(const RenderPassSpecification &spec);
    };

} // namespace Monado

#pragma once

#include "monado/core/base.h"

#include "framebuffer.h"

namespace Monado {

    struct RenderPassSpecification {
        Ref<Framebuffer> TargetFramebuffer;
    };

    class RenderPass {
    public:
        virtual ~RenderPass() {}

        virtual const RenderPassSpecification &GetSpecification() const = 0;

        static Ref<RenderPass> Create(const RenderPassSpecification &spec);
    };

} // namespace Monado

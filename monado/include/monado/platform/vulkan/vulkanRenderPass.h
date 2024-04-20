#pragma once

#include "monado/renderer/renderPass.h"

namespace Monado {

    class VulkanRenderPass : public RenderPass {
    public:
        VulkanRenderPass(const RenderPassSpecification &spec);
        virtual ~VulkanRenderPass();

        virtual RenderPassSpecification &GetSpecification() override { return m_Specification; }
        virtual const RenderPassSpecification &GetSpecification() const override { return m_Specification; }

    private:
        RenderPassSpecification m_Specification;
    };

} // namespace Monado

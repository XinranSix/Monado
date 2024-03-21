#pragma once

#include "monado/renderer/framebuffer.h"

namespace Monado {

    class OpenGLFramebuffer : public Framebuffer {
    public:
        OpenGLFramebuffer(const FramebufferSpecification &spec);
        virtual ~OpenGLFramebuffer();

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void BindTexture(uint32_t slot = 0) const override;

        virtual RendererID GetRendererID() const override { return m_RendererID; }
        virtual RendererID GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
        virtual RendererID GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

        virtual const FramebufferSpecification &GetSpecification() const override { return m_Specification; }

    private:
        FramebufferSpecification m_Specification;
        RendererID m_RendererID = 0;
        RendererID m_ColorAttachment, m_DepthAttachment;
    };

} // namespace Monado

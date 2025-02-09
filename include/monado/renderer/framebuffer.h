#pragma once

#include "monado/core/base.h"

#include <vector>

namespace Monado {

    // 帧缓冲纹理格式，包括颜色附件和深度附件
    enum class FramebufferTextureFormat {
        None = 0,
        // Color
        RGBA8,
        RED_INTEGER,
        // Depth/stencil
        DEPTH24STENCIL8,
        // Defaults
        Depth = DEPTH24STENCIL8
    };

    // 创建缓冲区要指定的格式
    struct FramebufferTextureSpecification {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat { format } {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
    };

    // 缓冲区附加到帧缓冲中要指定的格式 - 可以认为只是多一层封装 FramebufferTextureSpecification 成 vector
    struct FramebufferAttachmentSpecification {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments { attachments } {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferSpecification {
        uint32_t Width = 0, Height = 0;
        FramebufferAttachmentSpecification Attachments;
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual const FramebufferSpecification &GetSpecification() const = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification &spec);
    };

} // namespace Monado

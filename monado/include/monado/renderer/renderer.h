#pragma once

#include "renderCommandQueue.h"
#include "rendererAPI.h"
#include "monado/core/base.h"
#include "renderPass.h"
#include "monado/renderer/mesh.h"

namespace Monado {

    class ShaderLibrary;

    // TODO: Maybe this should be renamed to RendererAPI? Because we want an actual renderer vs API calls...
    class Renderer {
    public:
        typedef void (*RenderCommandFn)(void *);

        // Commands
        static void Clear();
        static void Clear(float r, float g, float b, float a = 1.0f);
        static void SetClearColor(float r, float g, float b, float a);

        static void DrawIndexed(uint32_t count, bool depthTest = true);

        static void ClearMagenta();

        static void Init();

        static const Scope<ShaderLibrary> &GetShaderLibrary() { return Get().m_ShaderLibrary; }

        static void *Submit(RenderCommandFn fn, unsigned int size) {
            return s_Instance->m_CommandQueue.Allocate(fn, size);
        }

        void WaitAndRender();

        inline static Renderer &Get() { return *s_Instance; }

        // ~Actual~ Renderer here... TODO: remove confusion later
        static void BeginRenderPass(const Ref<RenderPass> &renderPass) { s_Instance->IBeginRenderPass(renderPass); }
        static void EndRenderPass() { s_Instance->IEndRenderPass(); }

        static void SubmitMesh(const Ref<Mesh> &mesh) { s_Instance->SubmitMeshI(mesh); }

    private:
        void IBeginRenderPass(const Ref<RenderPass> &renderPass);
        void IEndRenderPass();

        void SubmitMeshI(const Ref<Mesh> &mesh);

    private:
        static Renderer *s_Instance;

    private:
        Ref<RenderPass> m_ActiveRenderPass;
        RenderCommandQueue m_CommandQueue;
        Scope<ShaderLibrary> m_ShaderLibrary;
    };

} // namespace Monado

#define MND_RENDER_PASTE2(a, b) a##b
#define MND_RENDER_PASTE(a, b) MND_RENDER_PASTE2(a, b)
#define MND_RENDER_UNIQUE(x) MND_RENDER_PASTE(x, __LINE__)

#define MND_RENDER(code)                                                                                               \
    struct MND_RENDER_UNIQUE(MNDRenderCommand) {                                                                       \
        static void Execute(void *) { code }                                                                           \
    };                                                                                                                 \
    {                                                                                                                  \
        auto mem = ::Monado::Renderer::Submit(MND_RENDER_UNIQUE(MNDRenderCommand)::Execute,                            \
                                              sizeof(MND_RENDER_UNIQUE(MNDRenderCommand)));                            \
        new (mem) MND_RENDER_UNIQUE(MNDRenderCommand)();                                                               \
    }

#define MND_RENDER_1(arg0, code)                                                                                       \
    do {                                                                                                               \
        struct MND_RENDER_UNIQUE(MNDRenderCommand) {                                                                   \
            MND_RENDER_UNIQUE(MNDRenderCommand)                                                                        \
            (typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0)          \
                : arg0(arg0) {}                                                                                        \
                                                                                                                       \
            static void Execute(void *argBuffer) {                                                                     \
                auto &arg0 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg0;                               \
                code                                                                                                   \
            }                                                                                                          \
                                                                                                                       \
            typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;           \
        };                                                                                                             \
        {                                                                                                              \
            auto mem = ::Monado::Renderer::Submit(MND_RENDER_UNIQUE(MNDRenderCommand)::Execute,                        \
                                                  sizeof(MND_RENDER_UNIQUE(MNDRenderCommand)));                        \
            new (mem) MND_RENDER_UNIQUE(MNDRenderCommand)(arg0);                                                       \
        }                                                                                                              \
    } while (0)

#define MND_RENDER_2(arg0, arg1, code)                                                                                 \
    struct MND_RENDER_UNIQUE(MNDRenderCommand) {                                                                       \
        MND_RENDER_UNIQUE(MNDRenderCommand)                                                                            \
        (typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1)              \
            : arg0(arg0), arg1(arg1) {}                                                                                \
                                                                                                                       \
        static void Execute(void *argBuffer) {                                                                         \
            auto &arg0 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg0;                                   \
            auto &arg1 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg1;                                   \
            code                                                                                                       \
        }                                                                                                              \
                                                                                                                       \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;               \
    };                                                                                                                 \
    {                                                                                                                  \
        auto mem = ::Monado::Renderer::Submit(MND_RENDER_UNIQUE(MNDRenderCommand)::Execute,                            \
                                              sizeof(MND_RENDER_UNIQUE(MNDRenderCommand)));                            \
        new (mem) MND_RENDER_UNIQUE(MNDRenderCommand)(arg0, arg1);                                                     \
    }

#define MND_RENDER_3(arg0, arg1, arg2, code)                                                                           \
    struct MND_RENDER_UNIQUE(MNDRenderCommand) {                                                                       \
        MND_RENDER_UNIQUE(MNDRenderCommand)                                                                            \
        (typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2)              \
            : arg0(arg0), arg1(arg1), arg2(arg2) {}                                                                    \
                                                                                                                       \
        static void Execute(void *argBuffer) {                                                                         \
            auto &arg0 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg0;                                   \
            auto &arg1 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg1;                                   \
            auto &arg2 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg2;                                   \
            code                                                                                                       \
        }                                                                                                              \
                                                                                                                       \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;               \
    };                                                                                                                 \
    {                                                                                                                  \
        auto mem = ::Monado::Renderer::Submit(MND_RENDER_UNIQUE(MNDRenderCommand)::Execute,                            \
                                              sizeof(MND_RENDER_UNIQUE(MNDRenderCommand)));                            \
        new (mem) MND_RENDER_UNIQUE(MNDRenderCommand)(arg0, arg1, arg2);                                               \
    }

#define MND_RENDER_4(arg0, arg1, arg2, arg3, code)                                                                     \
    struct MND_RENDER_UNIQUE(MNDRenderCommand) {                                                                       \
        MND_RENDER_UNIQUE(MNDRenderCommand)                                                                            \
        (typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2,              \
         typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3)              \
            : arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {}                                                        \
                                                                                                                       \
        static void Execute(void *argBuffer) {                                                                         \
            auto &arg0 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg0;                                   \
            auto &arg1 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg1;                                   \
            auto &arg2 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg2;                                   \
            auto &arg3 = ((MND_RENDER_UNIQUE(MNDRenderCommand) *)argBuffer) -> arg3;                                   \
            code                                                                                                       \
        }                                                                                                              \
                                                                                                                       \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;               \
        typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3;               \
    };                                                                                                                 \
    {                                                                                                                  \
        auto mem = Renderer::Submit(MND_RENDER_UNIQUE(MNDRenderCommand)::Execute,                                      \
                                    sizeof(MND_RENDER_UNIQUE(MNDRenderCommand)));                                      \
        new (mem) MND_RENDER_UNIQUE(MNDRenderCommand)(arg0, arg1, arg2, arg3);                                         \
    }

#define MND_RENDER_S(code)                                                                                             \
    auto self = this;                                                                                                  \
    MND_RENDER_1(self, code)

#define MND_RENDER_S1(arg0, code)                                                                                      \
    auto self = this;                                                                                                  \
    MND_RENDER_2(self, arg0, code)

#define MND_RENDER_S2(arg0, arg1, code)                                                                                \
    auto self = this;                                                                                                  \
    MND_RENDER_3(self, arg0, arg1, code)

#define MND_RENDER_S3(arg0, arg1, arg2, code)                                                                          \
    auto self = this;                                                                                                  \
    MND_RENDER_4(self, arg0, arg1, arg2, code)

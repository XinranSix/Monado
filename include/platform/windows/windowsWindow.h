#pragma once

#include "monado/core/window.h"
#include "monado/renderer/graphicsContext.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {
    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProps &props);
        virtual ~WindowsWindow();
        void OnUpdate() override;
        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }
        // 设置Application的回调函数
        inline void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override { return m_Data.VSync; }
        void *GetNativeWindow() const override;
        void OnResized(int width, int height) override;

    private:
        virtual void Init(const WindowProps &props);
        virtual void Shutdown();

    private:
        GLFWwindow *m_Window;
        GraphicsContext *m_Context;

        struct WindowData {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;
            EventCallbackFn EventCallback;
        };
        WindowData m_Data;
    };
} // namespace Monado

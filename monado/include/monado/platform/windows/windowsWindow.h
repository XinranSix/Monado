#pragma once

#include "monado/core/window.h"

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

        virtual std::pair<float, float> GetWindowPos() const override;

        // Window attributes
        inline void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline void *GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const WindowProps &props);
        virtual void Shutdown();

    private:
        GLFWwindow *m_Window;
        GLFWcursor *m_ImGuiMouseCursors[9] = { 0 };

        struct WindowData {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
        float m_LastFrameTime = 0.0f;
    };
} // namespace Monado

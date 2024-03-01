#pragma once

#include "core.h"
#include "monado/event/event.h"
#include "monado/renderer/graphicsContext.h"

namespace Monado {
    struct WindowProps {
    public:
        std::string title;
        unsigned int width;
        unsigned int height;

        WindowProps(const std::string &m_title = "Monado Engine", unsigned int w = 1280, unsigned int h = 720)
            : title { m_title }, width { w }, height { h } {}
    };

    class Window {
    public:
        // Window自带一个回调函数，用来处理从glfw库收到的callback
        using EventCallbackFn = std::function<void(Event &)>;
        virtual ~Window() {};
        virtual int const GetWindowHeight() const = 0;
        virtual int const GetWindowWidth() const = 0;
        virtual bool IsVSync() const = 0;
        virtual void SetVSync(bool) = 0;
        virtual void OnUpdate() = 0;
        virtual void OnResized(int width, int height) = 0;
        virtual void *GetNativeWindow() const = 0;
        virtual void SetEventCallback(const EventCallbackFn &callback) = 0;

        static Window *Create(const WindowProps &props = WindowProps {});
        GraphicsContext *m_Context;
    };
} // namespace Monado

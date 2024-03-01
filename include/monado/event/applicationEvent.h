#pragma once

#include "event.h"
#include <sstream>

// Windows和APP相关的Event都在这里处理
namespace Monado {
    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}
        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_GET_CATEGORY(EventCategoryApplication)

        std::string ToString() const override {
            std::stringstream a;
            a << "Window Close";
            return a.str();
        }

    protected:
    };

    class WindowResizedEvent : public Event {
    public:
        WindowResizedEvent(int height, int width) : m_Height { height }, m_Width { width } {}
        EVENT_CLASS_TYPE(WindowResized)
        EVENT_CLASS_GET_CATEGORY(EventCategoryApplication)

        inline int GetWindowHeight() { return m_Height; }
        inline int GetWindowWidth() { return m_Width; }
        std::string ToString() const override {
            std::stringstream a;
            a << "Window Resize: width = " << m_Width << ", height = " << m_Height;
            return a.str();
        }

    protected:
        int m_Height, m_Width;
    };

    class AppTickEvent : public Event {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_GET_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_GET_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public Event {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_GET_CATEGORY(EventCategoryApplication)
    };

} // namespace Monado

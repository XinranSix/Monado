#pragma once

#include <string>
#include <functional>

#include "monado/core/core.h"

#define EVENT_CLASS_TYPE(type)                                                                                         \
    static EventType GetStaticType() { return EventType::type; }                                                     \
    const EventType GetEventType() const override { return GetStaticType(); }                                          \
    const char *GetName() const override { return #type; }

#define EVENT_CLASS_GET_CATEGORY(category)                                                                             \
    int GetCategoryFlag() const override { return category; }

namespace Monado {
    // 事件的详细类型
    enum class EventType {
        None = 0,
        WindowClose,
        WindowResized,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,
        AppTick,
        AppUpdate,
        AppRender,
        KeyPressed,
        KeyReleased,
        KeyTyped,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    // 事件的粗略种类
    enum EventCategory {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4),
    };

    class Event {
    public:
        friend class EventDispatcher;
        virtual const char *GetName() const = 0;
        virtual const EventType GetEventType() const = 0;
        virtual int GetCategoryFlag() const = 0;
        virtual std::string ToString() const { return GetName(); }
        inline bool IsInCategory(EventCategory type) { return GetCategoryFlag() & type; }

        bool IsHandled() { return m_Handled; }
        void MarkHandled() { m_Handled = true; }

    protected:
        bool m_Handled = false; // 用来标记这个事件是否被处理了
    };

    // 当收到 Event 时，创建对应的 EventDispatcher
    // EventDispatcher 本质上是一个产生的 Event 的 Wrapper
    // 根据 wrap 的 Event 类型产生了对应的 Dispatch 事件的函数
    class EventDispatcher {
        template <typename T>
        using EventHandler =
            std::function<bool(T &)>; // EventHandler 存储了一个输入为任意类型的引用，返回值为bool的函数指针
    public:
        EventDispatcher(Event &e) : m_Event { e } {}

        // Dispatch会直接执行响应事件对应的函数指针对应的函数
        // T 指的是事件类型, 如果输入的类型没有GetStaticType会报错
        template <typename T>
        void Dispatch(EventHandler<T> handler) {
            if (m_Event.m_Handled) {
                return;
            }

            // 只有 Event 类型跟模板 T 匹配时, 才响应事件
            if (m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.m_Handled = handler(*(T *)&m_Event); // 使用(T*)把m_Event转换成输入事件的指针类型
                m_Event.m_Handled = true; // Temporary: 现在不会直接对应的Handler里都返回true
            }
        }

    private:
        Event &m_Event; // 必须是引用，不可以是Event的实例，因为Event带有纯虚函数
    };
} // namespace Monado

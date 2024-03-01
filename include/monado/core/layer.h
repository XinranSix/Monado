#pragma once

#include "core.h"
#include "monado/event/event.h"
#include "timestep.h"

namespace Monado {
    class Layer {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();
        virtual void OnAttach() {}; // 当 layer 添加到 layer stack 的时候会调用此函数，相当于 Init 函数
        virtual void OnDetach() {}; // 当 layer 从 layer stack 移除的时候会调用此函数，相当于 Shutdown 函数
        virtual void OnEvent(Event &) {};
        virtual void OnUpdate(const Timestep &) {};
        virtual void OnImGuiRender() {};

    protected:
        bool isEnabled; // 值为fasle时，该Layer会被禁用，不会绘制画面，也不会接收事件
        std::string m_DebugName;
    };

} // namespace Monado

#pragma once

#include "monado/core/core.h"
#include "monado/event/event.h"
#include "monado/core/timestep.h"

namespace Monado {
    class Layer {
    public:
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}            // 应用添加此层执行
        virtual void OnDetach() {}            // 应用分离此层执行
        virtual void OnUpdate(Timestep ts) {} // 每层更新
        virtual void OnImGuiRender() {}       // 每层都可以拥有自己的UI窗口 !
        virtual void OnEvent(Event &event) {} // 每层处理事件
        inline const std::string &GetName() const { return m_DebugName; }

    protected:
        bool isEnabled;
        std::string m_DebugName;
    };
} // namespace Monado

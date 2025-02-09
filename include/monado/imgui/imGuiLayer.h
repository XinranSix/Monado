#pragma once

#include "monado/core/layer.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"

namespace Monado {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event &e) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void SetDarkThemeColors();

        uint32_t GetActiveWidgetID() const;

    private:
        bool m_BlockEvents = true;
    };

} // namespace Monado

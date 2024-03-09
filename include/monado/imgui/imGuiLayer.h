#pragma once

#include "monado/core/layer.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"

namespace Monado {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event &event) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void SetDarkThemeColors();

    private:
      /*   // FIXME: 临时的
        bool OnMouseCursorMoved(MouseMovedEvent &e);

        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

        bool OnMouseButtonReleased(MouseButtonReleasedEvent &e);
 */
    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;
    };

} // namespace Monado

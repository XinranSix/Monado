#pragma once

#include "monado/core/base.h"
#include "monado/core/log.h"
#include "monado/scene/scene.h"
#include "monado/scene/entity.h"

namespace Monado {

    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene> &scene);

        void SetContext(const Ref<Scene> &scene);

        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Entity entity);

    private:
        template <typename T>
        void DisplayAddComponentEntry(const std::string &entryName);

        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };

} // namespace Monado

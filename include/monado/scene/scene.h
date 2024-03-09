#pragma once

#include "monado/core/timestep.h"
#include "monado/renderer/editorCamera.h"
#include "entt/entt.hpp"
#include <cstdint>

namespace Monado {
    class Entity;
    class SceneHierarchyPanel;
    class SerializeEntity;
    class Scene {
        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;

    public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        void DuplicateEntity(Entity entity);

        Entity CreateEnitty(std::string name);

        void DestroyEntity(Entity entity);

        void OnRuntimeStart();

        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        void OnUpdateRuntime(Timestep ts);
        void OnUpdateSimulation(Timestep ts, EditorCamera &camera);
        void OnUpdateEditor(Timestep ts, EditorCamera &camera);

        void OnViewportResize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();

        template <typename... Components>
        auto GetAllEntitiesWith() {
            return m_Registry.view<Components...>();
        }

        std::string GetCurFilePath() { return filepath; }
        void SetCurFilePath(const std::string &path) { filepath = path; }

        void OnUpdate(Timestep ts);

    private:
        template <typename T>
        void OnComponentAdded(Entity entity, T &component);

        void OnPhysics2DStart();
        void OnPhysics2DStop();

    private:
        std::string filepath;
        entt::registry m_Registry; // entt提供的注册表
        uint32_t m_ViewportWidth, m_ViewportHeight;
    };
} // namespace Monado

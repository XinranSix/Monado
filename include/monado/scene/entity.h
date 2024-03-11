#pragma once

#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "monado/core/uuid.h"
#include "components.h"
#include "scene.h"
#include "entt/entt.hpp"
#include <utility>

namespace Monado {
    class Scene;
    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene *scene);
        Entity(const Entity &other) = default;

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args) {
            MND_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T &component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T, typename... Args>
        T &AddOrReplaceComponent(Args &&...args) {
            T &component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T>
        T &GetComponent() {
            MND_CORE_ASSERT(HasComponent<T>(), "实体不存在这个组件");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }
        template <typename T>
        bool HasComponent() {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }
        template <typename T>
        void RemoveComponent() {
            MND_CORE_ASSERT(HasComponent<T>(), "实体不存在这个组件");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        operator bool() const { return m_EntityHandle != entt::null; }

        operator entt::entity() const { return m_EntityHandle; }

        UUID GetUUID() { return GetComponent<IDComponent>().ID; }
        const std::string &GetName() { return GetComponent<TagComponent>().Tag; }

        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        bool operator==(const Entity &other) const {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }

        bool operator!=(const Entity &other) const { return !(*this == other); }

    private:
        entt::entity m_EntityHandle { entt::null };
        Scene *m_Scene {};
    };
} // namespace Monado

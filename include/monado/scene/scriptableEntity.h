#pragma once

#include "monado/core/timestep.h"
#include "entity.h"

namespace Monado {
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() {}

        template <typename T>
        T &GetComponent() {
            return m_Entity.GetComponent<T>(); // 根据Entity类找到关联的组件
        }
        virtual void OnUpdate(Timestep ts) {}

    protected:
        // 这里，虚函数/
        virtual void OnCreate() {}
        virtual void OnDestroy() {}

    private:
        Entity m_Entity;
        friend class Scene; // 为了在scene中设置m_Entity
    };

} // namespace Monado

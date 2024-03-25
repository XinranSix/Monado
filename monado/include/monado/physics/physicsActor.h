#pragma once

#include "monado/scene/entity.h"

namespace physx {
    class PxRigidActor;
}

namespace Monado {

    class PhysicsActor : public RefCounted {
    public:
        PhysicsActor(Entity entity);
        ~PhysicsActor();

        void Update(float fixedTimestep);
        void SynchronizeTransform();

        void SetLayer(uint32_t layer);

        bool IsDynamic() const { return m_RigidBody.BodyType == RigidBodyComponent::Type::Dynamic; }

        Entity &GetEntity() { return m_Entity; }

    private:
        void Create();

        void SetRuntimeDataInternal(void *entityStorage, int storageBufferPosition);

    private:
        Entity m_Entity;
        RigidBodyComponent &m_RigidBody;
        PhysicsMaterialComponent m_Material;

        physx::PxRigidActor *m_ActorInternal;

        friend class Physics;
        friend class PXPhysicsWrappers;
    };

} // namespace Monado

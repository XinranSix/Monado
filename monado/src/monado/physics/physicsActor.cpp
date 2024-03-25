#include "monado/physics/physicsActor.h"
#include "monado/physics/physicsUtil.h"
#include "monado/physics/physics.h"
#include "monado/physics/pxPhysicsWrappers.h"
#include "monado/physics/physicsLayer.h"
#include "monado/script/scriptEngine.h"

#include "PxPhysicsAPI.h"

namespace Monado {

    PhysicsActor::PhysicsActor(Entity entity)
        : m_Entity(entity), m_RigidBody(entity.GetComponent<RigidBodyComponent>()) {
        m_Material = entity.GetComponent<PhysicsMaterialComponent>();

        Create();
    }

    PhysicsActor::~PhysicsActor() {
        m_ActorInternal->release();
        m_ActorInternal = nullptr;
    }

    void PhysicsActor::Create() {
        physx::PxPhysics &physics = PXPhysicsWrappers::GetPhysics();

        if (m_RigidBody.BodyType == RigidBodyComponent::Type::Static) {
            m_ActorInternal = physics.createRigidStatic(ToPhysXTransform(m_Entity.Transform()));
        } else {
            const PhysicsSettings &settings = Physics::GetSettings();

            physx::PxRigidDynamic *actor = physics.createRigidDynamic(ToPhysXTransform(m_Entity.Transform()));
            actor->setLinearDamping(m_RigidBody.LinearDrag);
            actor->setAngularDamping(m_RigidBody.AngularDrag);
            actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, m_RigidBody.IsKinematic);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, m_RigidBody.LockPositionX);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, m_RigidBody.LockPositionY);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, m_RigidBody.LockPositionZ);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_RigidBody.LockRotationX);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_RigidBody.LockRotationY);
            actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_RigidBody.LockRotationZ);
            actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, m_RigidBody.DisableGravity);
            actor->setSolverIterationCounts(settings.SolverIterations, settings.SolverVelocityIterations);

            physx::PxRigidBodyExt::updateMassAndInertia(*actor, m_RigidBody.Mass);
            m_ActorInternal = actor;
        }

        physx::PxMaterial *physicsMat =
            physics.createMaterial(m_Material.StaticFriction, m_Material.DynamicFriction, m_Material.Bounciness);
        if (m_Entity.HasComponent<BoxColliderComponent>())
            PXPhysicsWrappers::AddBoxCollider(*this, *physicsMat);
        if (m_Entity.HasComponent<SphereColliderComponent>())
            PXPhysicsWrappers::AddSphereCollider(*this, *physicsMat);
        if (m_Entity.HasComponent<CapsuleColliderComponent>())
            PXPhysicsWrappers::AddCapsuleCollider(*this, *physicsMat);
        if (m_Entity.HasComponent<MeshColliderComponent>())
            PXPhysicsWrappers::AddMeshCollider(*this, *physicsMat);

        if (!PhysicsLayerManager::IsLayerValid(m_RigidBody.Layer))
            m_RigidBody.Layer = 0;

        SetLayer(m_RigidBody.Layer);

        static_cast<physx::PxScene *>(Physics::GetPhysicsScene())->addActor(*m_ActorInternal);
    }

    void PhysicsActor::SetRuntimeDataInternal(void *entityStorage, int storageBufferPosition) {
        m_ActorInternal->userData = entityStorage;
        m_RigidBody.RuntimeActor = m_ActorInternal;
        m_RigidBody.EntityBufferIndex = storageBufferPosition;
    }

    void PhysicsActor::Update(float fixedTimestep) {
        if (!ScriptEngine::IsEntityModuleValid(m_Entity))
            return;

        ScriptEngine::OnPhysicsUpdateEntity(m_Entity, fixedTimestep);
    }

    void PhysicsActor::SynchronizeTransform() {
        TransformComponent &transform = m_Entity.Transform();
        physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(m_RigidBody.RuntimeActor);
        physx::PxTransform actorPose = actor->getGlobalPose();
        transform.Translation = FromPhysXVector(actorPose.p);
        transform.Rotation = glm::eulerAngles(FromPhysXQuat(actorPose.q));
    }

    void PhysicsActor::SetLayer(uint32_t layer) {
        physx::PxAllocatorCallback &allocator = PXPhysicsWrappers::GetAllocator();

        const PhysicsLayer &layerInfo = PhysicsLayerManager::GetLayer(layer);

        if (layerInfo.CollidesWith == 0)
            return;

        physx::PxFilterData filterData;
        filterData.word0 = layerInfo.BitValue;
        filterData.word1 = layerInfo.CollidesWith;

        const physx::PxU32 numShapes = m_ActorInternal->getNbShapes();

        physx::PxShape **shapes =
            (physx::PxShape **)allocator.allocate(sizeof(physx::PxShape *) * numShapes, "", "", 0);
        m_ActorInternal->getShapes(shapes, numShapes);

        for (physx::PxU32 i = 0; i < numShapes; i++)
            shapes[i]->setSimulationFilterData(filterData);

        allocator.deallocate(shapes);
    }

} // namespace Hazel

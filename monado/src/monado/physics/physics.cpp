
#include "monado/physics/physics.h"
#include "monado/physics/pXPhysicsWrappers.h"
#include "monado/physics/physicsLayer.h"

#include <cstdint>

namespace Monado {

    static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4 &transform) {
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(transform, scale, orientation, translation, skew, perspective);

        return { translation, orientation, scale };
    }

    static physx::PxScene *s_Scene;
    static std::vector<Entity> s_SimulatedEntities;
    static Entity *s_EntityStorageBuffer;
    static uint32_t s_EntityBufferCount;
    static int s_EntityStorageBufferPosition;
    static float s_SimulationTime = 0.0F;

    static PhysicsSettings s_Settings;

    void Physics::Init() {
        PXPhysicsWrappers::Initialize();
        PhysicsLayerManager::AddLayer("Default");
    }

    void Physics::Shutdown() { PXPhysicsWrappers::Shutdown(); }

    void Physics::ExpandEntityBuffer(uint32_t amount) {
        MND_CORE_ASSERT(s_Scene);

        if (s_EntityStorageBuffer != nullptr) {
            Entity *temp = new Entity[s_EntityBufferCount + amount];
            memcpy(temp, s_EntityStorageBuffer, s_EntityBufferCount * sizeof(Entity));

            for (uint32_t i = 0; i < s_EntityBufferCount; i++) {
                Entity &e = s_EntityStorageBuffer[i];
                RigidBodyComponent &rb = e.GetComponent<RigidBodyComponent>();

                if (rb.RuntimeActor) {
                    physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(rb.RuntimeActor);
                    actor->userData = &temp[rb.EntityBufferIndex];
                }
            }

            delete[] s_EntityStorageBuffer;
            s_EntityStorageBuffer = temp;
            s_EntityBufferCount += amount;
        } else {
            s_EntityStorageBuffer = new Entity[amount];
            s_EntityBufferCount = amount;
        }
    }

    void Physics::CreateScene() {
        MND_CORE_ASSERT(s_Scene == nullptr, "Scene already has a Physics Scene!");
        s_Scene = PXPhysicsWrappers::CreateScene();
    }

    void Physics::CreateActor(Entity e) {
        MND_CORE_ASSERT(s_Scene);

        if (!e.HasComponent<RigidBodyComponent>()) {
            MND_CORE_WARN("Trying to create PhysX actor from a non-rigidbody actor!");
            return;
        }

        if (!e.HasComponent<PhysicsMaterialComponent>()) {
            MND_CORE_WARN("Trying to create PhysX actor without a PhysicsMaterialComponent!");
            return;
        }

        RigidBodyComponent &rigidbody = e.GetComponent<RigidBodyComponent>();

        physx::PxRigidActor *actor = PXPhysicsWrappers::CreateActor(rigidbody, e.Transform());

        if (rigidbody.BodyType == RigidBodyComponent::Type::Dynamic)
            s_SimulatedEntities.push_back(e);

        Entity *entityStorage = &s_EntityStorageBuffer[s_EntityStorageBufferPosition];
        *entityStorage = e;
        actor->userData = (void *)entityStorage;
        rigidbody.RuntimeActor = actor;
        rigidbody.EntityBufferIndex = s_EntityStorageBufferPosition;
        s_EntityStorageBufferPosition++;

        physx::PxMaterial *material = PXPhysicsWrappers::CreateMaterial(e.GetComponent<PhysicsMaterialComponent>());

        const auto &transform = e.Transform();
        auto [translation, rotation, scale] = GetTransformDecomposition(transform);

        if (e.HasComponent<BoxColliderComponent>()) {
            BoxColliderComponent &collider = e.GetComponent<BoxColliderComponent>();
            PXPhysicsWrappers::AddBoxCollider(*actor, *material, collider, scale);
        }

        if (e.HasComponent<SphereColliderComponent>()) {
            SphereColliderComponent &collider = e.GetComponent<SphereColliderComponent>();
            PXPhysicsWrappers::AddSphereCollider(*actor, *material, collider, scale);
        }

        if (e.HasComponent<CapsuleColliderComponent>()) {
            CapsuleColliderComponent &collider = e.GetComponent<CapsuleColliderComponent>();
            PXPhysicsWrappers::AddCapsuleCollider(*actor, *material, collider, scale);
        }

        if (e.HasComponent<MeshColliderComponent>()) {
            MeshColliderComponent &collider = e.GetComponent<MeshColliderComponent>();
            PXPhysicsWrappers::AddMeshCollider(*actor, *material, collider, scale);
        }

        if (!PhysicsLayerManager::IsLayerValid(rigidbody.Layer))
            rigidbody.Layer = 0;

        PXPhysicsWrappers::SetCollisionFilters(*actor, rigidbody.Layer);

        s_Scene->addActor(*actor);
    }

    PhysicsSettings &Physics::GetSettings() { return s_Settings; }

    void Physics::Simulate(Timestep ts) {
        MND_CORE_ASSERT(s_Scene);

        s_SimulationTime += ts.GetMilliseconds();

        if (s_SimulationTime < s_Settings.FixedTimestep)
            return;

        s_SimulationTime -= s_Settings.FixedTimestep;

        s_Scene->simulate(s_Settings.FixedTimestep);
        s_Scene->fetchResults(true);

        for (Entity &e : s_SimulatedEntities) {
            auto &transform = e.Transform();
            auto [translation, rotation, scale] = GetTransformDecomposition(transform);
            RigidBodyComponent &rb = e.GetComponent<RigidBodyComponent>();
            physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(rb.RuntimeActor);
            transform = FromPhysXTransform(actor->getGlobalPose()) * glm::scale(glm::mat4(1.0F), scale);
        }
    }

    void Physics::DestroyScene() {
        MND_CORE_ASSERT(s_Scene);

        delete[] s_EntityStorageBuffer;
        s_EntityStorageBuffer = nullptr;
        s_EntityStorageBufferPosition = 0;
        s_SimulatedEntities.clear();
        s_Scene->release();
        s_Scene = nullptr;
    }

    void *Physics::GetPhysicsScene() { return s_Scene; }

} // namespace Monado

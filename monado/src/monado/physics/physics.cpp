
#include "monado/physics/physics.h"
#include "monado/physics/pXPhysicsWrappers.h"
#include "monado/physics/physicsLayer.h"
#include "monado/physics/PhysicsActor.h"
#include "monado/script/scriptEngine.h"

#include "extensions/PxBroadPhaseExt.h"

#include <cstdint>
#include <filesystem>

namespace Monado {

    static physx::PxScene *s_Scene;
    static std::vector<Ref<PhysicsActor>> s_SimulatedActors;
    static std::vector<Ref<PhysicsActor>> s_StaticActors;
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

    // NOTE: This is future proofing for when we can spawn entities from scripts/at runtime
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

        if (s_Settings.BroadphaseAlgorithm != BroadphaseType::AutomaticBoxPrune) {
            physx::PxBounds3 *regionBounds;
            physx::PxBounds3 globalBounds(ToPhysXVector(s_Settings.WorldBoundsMin),
                                          ToPhysXVector(s_Settings.WorldBoundsMax));
            uint32_t regionCount = physx::PxBroadPhaseExt::createRegionsFromWorldBounds(
                regionBounds, globalBounds, s_Settings.WorldBoundsSubdivisions);

            for (uint32_t i = 0; i < regionCount; i++) {
                physx::PxBroadPhaseRegion region;
                region.bounds = regionBounds[i];
                s_Scene->addBroadPhaseRegion(region);
            }
        }
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

        Ref<PhysicsActor> actor = Ref<PhysicsActor>::Create(e);

        if (actor->IsDynamic())
            s_SimulatedActors.push_back(actor);
        else
            s_StaticActors.push_back(actor);

        Entity *entityStorage = &s_EntityStorageBuffer[s_EntityStorageBufferPosition];
        *entityStorage = e;
        actor->SetRuntimeDataInternal((void *)entityStorage, s_EntityStorageBufferPosition);
        s_EntityStorageBufferPosition++;
    }

    PhysicsSettings &Physics::GetSettings() { return s_Settings; }

    void Physics::Simulate(Timestep ts) {
        s_SimulationTime += ts.GetMilliseconds();

        if (s_SimulationTime < s_Settings.FixedTimestep)
            return;

        s_SimulationTime -= s_Settings.FixedTimestep;

        for (auto &actor : s_SimulatedActors)
            actor->Update(s_Settings.FixedTimestep);

        s_Scene->simulate(s_Settings.FixedTimestep);
        s_Scene->fetchResults(true);

        for (auto &actor : s_SimulatedActors)
            actor->SynchronizeTransform();
    }

    void Physics::DestroyScene() {
        MND_CORE_ASSERT(s_Scene);

        delete[] s_EntityStorageBuffer;
        s_EntityStorageBuffer = nullptr;
        s_EntityStorageBufferPosition = 0;
        s_StaticActors.clear();
        s_SimulatedActors.clear();
        s_Scene->release();
        s_Scene = nullptr;
    }

    void *Physics::GetPhysicsScene() { return s_Scene; }

} // namespace Monado

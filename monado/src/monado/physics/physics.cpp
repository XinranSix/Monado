
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
    static float s_SimulationTime = 0.0F;

    static PhysicsSettings s_Settings;

    void Physics::Init() {
        PXPhysicsWrappers::Initialize();
        PhysicsLayerManager::AddLayer("Default");
    }

    void Physics::Shutdown() { PXPhysicsWrappers::Shutdown(); }

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

        actor->Spawn();
    }

    Ref<PhysicsActor> Physics::GetActorForEntity(Entity entity) {
        for (auto &actor : s_StaticActors) {
            if (actor->GetEntity() == entity)
                return actor;
        }

        for (auto &actor : s_SimulatedActors) {
            if (actor->GetEntity() == entity)
                return actor;
        }

        return nullptr;
    }

    PhysicsSettings &Physics::GetSettings() { return s_Settings; }

    // TODO: Physics Thread
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

        s_StaticActors.clear();
        s_SimulatedActors.clear();
        s_Scene->release();
        s_Scene = nullptr;
    }

    void *Physics::GetPhysicsScene() { return s_Scene; }
} // namespace Monado

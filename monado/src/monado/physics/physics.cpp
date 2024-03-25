
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
    static std::vector<Ref<PhysicsActor>> s_Actors;
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

    Ref<PhysicsActor> Physics::CreateActor(Entity e) {
        MND_CORE_ASSERT(s_Scene);
        Ref<PhysicsActor> actor = Ref<PhysicsActor>::Create(e);
        s_Actors.push_back(actor);
        actor->Spawn();
        return actor;
    }

    Ref<PhysicsActor> Physics::GetActorForEntity(Entity entity) {
        for (auto &actor : s_Actors) {
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

        for (auto &actor : s_Actors)
            actor->Update(s_Settings.FixedTimestep);

        s_Scene->simulate(s_Settings.FixedTimestep);
        s_Scene->fetchResults(true);

        for (auto &actor : s_Actors)
            actor->SynchronizeTransform();
    }

    void Physics::DestroyScene() {
        MND_CORE_ASSERT(s_Scene);

        s_Actors.clear();
        s_Scene->release();
        s_Scene = nullptr;
    }

    void *Physics::GetPhysicsScene() { return s_Scene; }
} // namespace Monado

#pragma once

#include "monado/core/base.h"
#include "monado/scene/entity.h"
#include "monado/core/timeStep.h"

namespace Monado {

    enum class ForceMode : uint16_t { Force = 0, Impulse, VelocityChange, Acceleration };

    enum class FilterGroup : uint32_t {
        Static = BIT(0),
        Dynamic = BIT(1),
        Kinematic = BIT(2),
        All = Static | Dynamic | Kinematic
    };

    enum class BroadphaseType {
        SweepAndPrune,
        MultiBoxPrune,
        AutomaticBoxPrune
        // TODO: GPU?
    };

    struct PhysicsSettings {
        float FixedTimestep = 0.02F;
        glm::vec3 Gravity = { 0.0F, -9.81F, 0.0F };
        BroadphaseType BroadphaseAlgorithm = BroadphaseType::AutomaticBoxPrune;
    };

    struct RaycastHit {
        uint64_t EntityID;
        glm::vec3 Position;
        glm::vec3 Normal;
        float Distance;
    };

    class Physics {
    public:
        static void Init();
        static void Shutdown();

        static void ExpandEntityBuffer(uint32_t entityCount);

        static void CreateScene();
        static void CreateActor(Entity e);

        static void Simulate(Timestep ts);

        static void DestroyScene();

        static void *GetPhysicsScene();

    public:
        static PhysicsSettings &GetSettings();
    };

} // namespace Monado

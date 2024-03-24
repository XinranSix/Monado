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

    struct SceneParams {
        glm::vec3 Gravity = { 0.0F, -9.81F, 0.0F };
    };

    struct PhysicsLayer {
        uint32_t LayerID;
        std::string Name;
        uint32_t BitValue;
        int32_t CollidesWith = 0;
    };

    class PhysicsLayerManager {
    public:
        static uint32_t AddLayer(const std::string &name, bool setCollisions = true);
        static void RemoveLayer(uint32_t layerId);

        static void SetLayerCollision(uint32_t layerId, uint32_t otherLayer, bool collides);
        static const std::vector<PhysicsLayer> &GetLayerCollisions(uint32_t layerId);

        static const std::vector<PhysicsLayer> &GetLayers() { return s_Layers; }

        static PhysicsLayer &GetLayer(uint32_t layerId);
        static PhysicsLayer &GetLayer(const std::string &layerName);
        static uint32_t GetLayerCount() { return s_Layers.size(); }

        static bool ShouldCollide(uint32_t layer1, uint32_t layer2);
        static bool IsLayerValid(uint32_t layerId);

        static void ClearLayers();

    private:
        static uint32_t GetNextLayerID();

    private:
        static std::vector<PhysicsLayer> s_Layers;
        static std::unordered_map<uint32_t, std::vector<PhysicsLayer>> s_LayerCollisions;
        static PhysicsLayer s_NullLayer;
    };

    class Physics {
    public:
        static void Init();
        static void Shutdown();

        static void CreateScene(const SceneParams &params);
        static void CreateActor(Entity e, int entityCount);

        static void Simulate(Timestep ts);

        static void DestroyScene();

        static void ConnectVisualDebugger();
        static void DisconnectVisualDebugger();
    };

} // namespace Monado

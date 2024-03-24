
#include "monado/physics/physics.h"
#include "monado/physics/pXPhysicsWrappers.h"
#include "monado/scene/scene.h"
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
    static int s_EntityStorageBufferPosition;
    static float s_SimulationTime = 0.0F;

    void Physics::Init() {
        PXPhysicsWrappers::Initialize();
        PhysicsLayerManager::AddLayer("Default");
    }

    void Physics::Shutdown() { PXPhysicsWrappers::Shutdown(); }

    void Physics::CreateScene(const SceneParams &params) {
        MND_CORE_ASSERT(s_Scene == nullptr, "Scene already has a Physics Scene!");
        s_Scene = PXPhysicsWrappers::CreateScene(params);
    }

    void Physics::CreateActor(Entity e, int entityCount) {
        if (!e.HasComponent<RigidBodyComponent>()) {
            MND_CORE_WARN("Trying to create PhysX actor from a non-rigidbody actor!");
            return;
        }

        if (!e.HasComponent<PhysicsMaterialComponent>()) {
            MND_CORE_WARN("Trying to create PhysX actor without a PhysicsMaterialComponent!");
            return;
        }

        RigidBodyComponent &rigidbody = e.GetComponent<RigidBodyComponent>();

        if (s_EntityStorageBuffer == nullptr)
            s_EntityStorageBuffer = new Entity[entityCount];

        physx::PxRigidActor *actor = PXPhysicsWrappers::CreateActor(rigidbody, e.Transform());
        s_SimulatedEntities.push_back(e);
        Entity *entityStorage = &s_EntityStorageBuffer[s_EntityStorageBufferPosition++];
        *entityStorage = e;
        actor->userData = (void *)entityStorage;
        rigidbody.RuntimeActor = actor;

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

        PXPhysicsWrappers::SetCollisionFilters(*actor, rigidbody.Layer);

        s_Scene->addActor(*actor);
    }

    void Physics::Simulate(Timestep ts) {
        constexpr float stepSize = 1.0F / 50.0F;
        s_SimulationTime += ts.GetMilliseconds();

        if (s_SimulationTime < stepSize)
            return;

        s_SimulationTime -= stepSize;

        s_Scene->simulate(stepSize);
        s_Scene->fetchResults(true);

        for (Entity &e : s_SimulatedEntities) {
            auto &transform = e.Transform();
            auto [translation, rotation, scale] = GetTransformDecomposition(transform);
            RigidBodyComponent &rb = e.GetComponent<RigidBodyComponent>();
            physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(rb.RuntimeActor);

            if (rb.BodyType == RigidBodyComponent::Type::Dynamic) {
                transform = FromPhysXTransform(actor->getGlobalPose()) * glm::scale(glm::mat4(1.0F), scale);
            } else if (rb.BodyType == RigidBodyComponent::Type::Static) {
                actor->setGlobalPose(ToPhysXTransform(transform));
            }
        }
    }

    void Physics::DestroyScene() {
        delete[] s_EntityStorageBuffer;
        s_EntityStorageBuffer = nullptr;
        s_EntityStorageBufferPosition = 0;
        s_SimulatedEntities.clear();
        s_Scene->release();
        s_Scene = nullptr;
    }

    void Physics::ConnectVisualDebugger() { PXPhysicsWrappers::ConnectVisualDebugger(); }

    void Physics::DisconnectVisualDebugger() { PXPhysicsWrappers::DisconnectVisualDebugger(); }

    //////////////////////// Physics Layers ////////////////////////

    template <typename T, typename ConditionFunction>
    static bool RemoveIfExists(std::vector<T> &vector, ConditionFunction condition) {
        for (typename std::vector<T>::iterator it = vector.begin(); it != vector.end(); ++it) {
            if (condition(*it)) {
                vector.erase(it);
                return true;
            }
        }

        return false;
    }

    uint32_t PhysicsLayerManager::AddLayer(const std::string &name, bool setCollisions) {
        uint32_t layerId = GetNextLayerID();
        PhysicsLayer layer = { layerId, name, (uint32_t)BIT(layerId) };
        s_Layers.push_back(layer);

        s_LayerCollisions[layerId] = std::vector<PhysicsLayer>();
        s_LayerCollisions[layerId].reserve(1);

        if (setCollisions) {
            for (const auto &layer2 : s_Layers) {
                SetLayerCollision(layer.LayerID, layer2.LayerID, true);
            }
        }

        return layer.LayerID;
    }

    void PhysicsLayerManager::RemoveLayer(uint32_t layerId) {
        if (!IsLayerValid(layerId))
            return;

        PhysicsLayer &layerInfo = GetLayer(layerId);

        for (auto &kv : s_LayerCollisions) {
            if (kv.first == layerId)
                continue;

            if (RemoveIfExists<PhysicsLayer>(kv.second,
                                             [&](const PhysicsLayer &layer) { return layer.LayerID == layerId; })) {
                GetLayer(kv.first).CollidesWith &= ~layerInfo.BitValue;
            }
        }

        s_LayerCollisions.erase(layerId);

        RemoveIfExists<PhysicsLayer>(s_Layers, [&](const PhysicsLayer &layer) { return layer.LayerID == layerId; });
    }

    void PhysicsLayerManager::SetLayerCollision(uint32_t layerId, uint32_t otherLayer, bool collides) {
        if (ShouldCollide(layerId, otherLayer) && collides)
            return;

        PhysicsLayer &layerInfo = GetLayer(layerId);
        PhysicsLayer &otherLayerInfo = GetLayer(otherLayer);

        if (collides) {
            s_LayerCollisions[layerId].push_back(otherLayerInfo);
            layerInfo.CollidesWith |= otherLayerInfo.BitValue;

            if (layerId == otherLayer)
                return;

            s_LayerCollisions[otherLayer].push_back(layerInfo);
            otherLayerInfo.CollidesWith |= layerInfo.BitValue;
        } else {
            RemoveIfExists(s_LayerCollisions[layerId],
                           [&](const PhysicsLayer &layer) { return layer.LayerID == otherLayer; });
            layerInfo.CollidesWith &= ~otherLayerInfo.BitValue;

            RemoveIfExists(s_LayerCollisions[otherLayer],
                           [&](const PhysicsLayer &layer) { return layer.LayerID == layerId; });
            otherLayerInfo.CollidesWith &= ~layerInfo.BitValue;
        }
    }

    const std::vector<PhysicsLayer> &PhysicsLayerManager::GetLayerCollisions(uint32_t layerId) {
        MND_CORE_ASSERT(s_LayerCollisions.find(layerId) != s_LayerCollisions.end());
        return s_LayerCollisions[layerId];
    }

    PhysicsLayer &PhysicsLayerManager::GetLayer(uint32_t layerId) {
        for (auto &layer : s_Layers) {
            if (layer.LayerID == layerId) {
                return layer;
            }
        }

        return s_NullLayer;
    }

    PhysicsLayer &PhysicsLayerManager::GetLayer(const std::string &layerName) {
        for (auto &layer : s_Layers) {
            if (layer.Name == layerName) {
                return layer;
            }
        }

        return s_NullLayer;
    }

    bool PhysicsLayerManager::ShouldCollide(uint32_t layer1, uint32_t layer2) {
        for (const auto &collidingLayer : s_LayerCollisions[layer1]) {
            if (collidingLayer.LayerID == layer2)
                return true;
        }

        return false;
    }

    bool PhysicsLayerManager::IsLayerValid(uint32_t layerId) {
        for (const auto &layer : s_Layers) {
            if (layer.LayerID == layerId)
                return true;
        }

        return false;
    }

    void PhysicsLayerManager::ClearLayers() {
        s_Layers.clear();
        s_LayerCollisions.clear();
    }

    uint32_t PhysicsLayerManager::GetNextLayerID() {
        int32_t lastId = -1;

        for (const auto &layer : s_Layers) {
            if (lastId != -1) {
                if (layer.LayerID != lastId + 1) {
                    return lastId + 1;
                }
            }

            lastId = layer.LayerID;
        }

        return s_Layers.size();
    }

    std::vector<PhysicsLayer> PhysicsLayerManager::s_Layers;
    std::unordered_map<uint32_t, std::vector<PhysicsLayer>> PhysicsLayerManager::s_LayerCollisions;
    PhysicsLayer PhysicsLayerManager::s_NullLayer = { 0, "NULL", 0, -1 };

} // namespace Monado

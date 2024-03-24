
#include "monado/physics/physics.h"
#include "monado/physics/pXPhysicsWrappers.h"
#include "monado/scene/scene.h"

namespace Monado {

    static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4 &transform) {
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(transform, scale, orientation, translation, skew, perspective);

        return { translation, orientation, scale };
    }

    static std::vector<std::string> s_LayerNames;

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

        s_LayerNames.push_back(name);
        return layer.LayerID;
    }

    void PhysicsLayerManager::RemoveLayer(uint32_t layerId) {
        if (!IsLayerValid(layerId))
            return;

        // TODO: Cleanup this code

        std::vector<std::string>::iterator it = s_LayerNames.begin();
        for (; it != s_LayerNames.end();) {
            if (*it == GetLayerInfo(layerId).Name) {
                s_LayerNames.erase(it);
                break;
            } else {
                ++it;
            }
        }

        for (auto &kv : s_LayerCollisions) {
            if (kv.first == layerId)
                continue;

            std::vector<PhysicsLayer>::iterator layerIt = kv.second.begin();
            for (; layerIt != kv.second.end();) {
                if (layerIt->LayerID == layerId) {
                    GetLayerInfo(kv.first).CollidesWith &= ~GetLayerInfo(layerId).BitValue;
                    kv.second.erase(layerIt);
                    break;
                } else {
                    ++layerIt;
                }
            }
        }

        s_LayerCollisions.erase(layerId);

        std::vector<PhysicsLayer>::iterator layerIt = s_Layers.begin();
        for (; layerIt != s_Layers.end();) {
            if (layerIt->LayerID == layerId) {
                s_Layers.erase(layerIt);
                break;
            } else {
                ++layerIt;
            }
        }
    }

    void PhysicsLayerManager::SetLayerCollision(uint32_t layerId, uint32_t otherLayer, bool collides) {
        if (ShouldCollide(layerId, otherLayer) && collides)
            return;

        PhysicsLayer &layerInfo = GetLayerInfo(layerId);
        PhysicsLayer &otherLayerInfo = GetLayerInfo(otherLayer);

        // TODO: Cleanup this code

        if (collides) {
            s_LayerCollisions[layerId].push_back(otherLayerInfo);
            layerInfo.CollidesWith |= otherLayerInfo.BitValue;

            if (layerId != otherLayer) {
                s_LayerCollisions[otherLayer].push_back(layerInfo);
                otherLayerInfo.CollidesWith |= layerInfo.BitValue;
            }
        } else {
            std::vector<PhysicsLayer>::iterator it = s_LayerCollisions[layerId].begin();
            for (; it != s_LayerCollisions[layerId].end();) {
                if (it->LayerID == otherLayer) {
                    s_LayerCollisions[layerId].erase(it);
                    layerInfo.CollidesWith &= ~otherLayerInfo.BitValue;
                    break;
                } else {
                    ++it;
                }
            }

            it = s_LayerCollisions[otherLayer].begin();
            for (; it != s_LayerCollisions[otherLayer].end();) {
                if (it->LayerID == layerId) {
                    s_LayerCollisions[otherLayer].erase(it);
                    otherLayerInfo.CollidesWith &= ~layerInfo.BitValue;
                    break;
                } else {
                    ++it;
                }
            }
        }
    }

    const std::vector<Monado::PhysicsLayer> &PhysicsLayerManager::GetLayerCollisions(uint32_t layerId) {
        MND_CORE_ASSERT(s_LayerCollisions.find(layerId) != s_LayerCollisions.end());
        return s_LayerCollisions[layerId];
    }

    PhysicsLayer &PhysicsLayerManager::GetLayerInfo(uint32_t layerId) {
        // MND_CORE_ASSERT(s_Layers.find(layerId) != s_Layers.end());
        for (auto &layer : s_Layers) {
            if (layer.LayerID == layerId) {
                return layer;
            }
        }

        return s_NullLayer;
    }

    Monado::PhysicsLayer &PhysicsLayerManager::GetLayerInfo(const std::string &layerName) {
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

    const std::vector<std::string> &PhysicsLayerManager::GetLayerNames() { return s_LayerNames; }

    void PhysicsLayerManager::ClearLayers() {
        s_Layers.clear();
        s_LayerCollisions.clear();
        s_LayerNames.clear();
    }

    void PhysicsLayerManager::Init() { AddLayer("Default"); }

    void PhysicsLayerManager::Shutdown() {}

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

    static physx::PxScene *s_Scene;
    static std::vector<Entity> s_SimulatedEntities;
    static Entity *s_EntityStorageBuffer;
    static int s_EntityStorageBufferPosition;
    static float s_SimulationTime = 0.0F;

    void Physics::Init() {
        PXPhysicsWrappers::Initialize();
        PhysicsLayerManager::Init();
        ;
    }

    void Physics::Shutdown() {
        PhysicsLayerManager::Shutdown();
        PXPhysicsWrappers::Shutdown();
    }

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

        // Create Actor Body
        physx::PxRigidActor *actor = PXPhysicsWrappers::CreateActor(rigidbody, e.Transform());
        s_SimulatedEntities.push_back(e);
        Entity *entityStorage = &s_EntityStorageBuffer[s_EntityStorageBufferPosition++];
        *entityStorage = e;
        actor->userData = (void *)entityStorage;
        rigidbody.RuntimeActor = actor;

        // Physics Material
        physx::PxMaterial *material = PXPhysicsWrappers::CreateMaterial(e.GetComponent<PhysicsMaterialComponent>());

        // Add all colliders

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

        // Set collision filters
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

} // namespace Monado

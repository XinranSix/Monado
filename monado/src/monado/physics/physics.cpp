
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
    static uint32_t s_LastLayerID = 0;

    uint32_t PhysicsLayerManager::AddLayer(const std::string &name) {
        PhysicsLayer layer = { s_LastLayerID, name, (uint32_t)BIT(s_LastLayerID) };
        s_Layers[s_LastLayerID] = layer;
        s_LastLayerID++;
        s_LayerNames.push_back(name);

        // TODO: We might not always want to do this
        SetLayerCollision(layer.LayerID, layer.LayerID, true);

        return layer.LayerID;
    }

    void PhysicsLayerManager::RemoveLayer(uint32_t layerId) {
        if (s_Layers.find(layerId) != s_Layers.end()) {
            for (std::vector<std::string>::iterator it = s_LayerNames.begin(); it != s_LayerNames.end(); it++) {
                if (*it == s_Layers[layerId].Name) {
                    s_LayerNames.erase(it--);
                }
            }

            s_Layers.erase(layerId);
        }
    }

    void PhysicsLayerManager::SetLayerCollision(uint32_t layerId, uint32_t otherLayer, bool collides) {
        if (s_LayerCollisions.find(layerId) == s_LayerCollisions.end()) {
            s_LayerCollisions[layerId] = std::vector<PhysicsLayer>();
            s_LayerCollisions[layerId].reserve(1);
        }

        if (s_LayerCollisions.find(otherLayer) == s_LayerCollisions.end()) {
            s_LayerCollisions[otherLayer] = std::vector<PhysicsLayer>();
            s_LayerCollisions[otherLayer].reserve(1);
        }

        if (collides) {
            s_LayerCollisions[layerId].push_back(GetLayerInfo(otherLayer));
            s_LayerCollisions[otherLayer].push_back(GetLayerInfo(layerId));
        } else {
            for (std::vector<PhysicsLayer>::iterator it = s_LayerCollisions[layerId].begin();
                 it != s_LayerCollisions[layerId].end(); it++) {
                if (it->LayerID == otherLayer) {
                    s_LayerCollisions[layerId].erase(it--);
                }
            }

            for (std::vector<PhysicsLayer>::iterator it = s_LayerCollisions[otherLayer].begin();
                 it != s_LayerCollisions[otherLayer].end(); it++) {
                if (it->LayerID == layerId) {
                    s_LayerCollisions[otherLayer].erase(it--);
                }
            }
        }
    }

    const std::vector<Monado::PhysicsLayer> &PhysicsLayerManager::GetLayerCollisions(uint32_t layerId) {
        MND_CORE_ASSERT(s_LayerCollisions.find(layerId) != s_LayerCollisions.end());
        return s_LayerCollisions[layerId];
    }

    const PhysicsLayer &PhysicsLayerManager::GetLayerInfo(uint32_t layerId) {
        MND_CORE_ASSERT(s_Layers.find(layerId) != s_Layers.end());
        return s_Layers[layerId];
    }

    const Monado::PhysicsLayer &PhysicsLayerManager::GetLayerInfo(const std::string &layerName) {
        for (const auto &kv : s_Layers) {
            if (kv.second.Name == layerName) {
                return kv.second;
            }
        }

        return {};
    }

    const std::vector<std::string> &PhysicsLayerManager::GetLayerNames() { return s_LayerNames; }

    void PhysicsLayerManager::ClearLayers() {
        s_Layers.clear();
        s_LayerCollisions.clear();
        s_LastLayerID = 0;
        s_LayerNames.clear();
    }

    void PhysicsLayerManager::Init() { AddLayer("Default"); }

    void PhysicsLayerManager::Shutdown() {}

    std::unordered_map<uint32_t, PhysicsLayer> PhysicsLayerManager::s_Layers;
    std::unordered_map<uint32_t, std::vector<PhysicsLayer>> PhysicsLayerManager::s_LayerCollisions;

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

#include "monado/physics/pxPhysicsWrappers.h"
#include "monado/physics/physics.h"
#include "monado/physics/physicsLayer.h"
#include "monado/physics/PhysicsActor.h"
#include "monado/script/scriptEngine.h"

#include "glm/gtx/rotate_vector.hpp"
#include <extensions/PxRigidActorExt.h>

namespace Monado {

    static PhysicsErrorCallback s_ErrorCallback;
    static PhysicsAssertHandler s_AssertHandler;
    static physx::PxDefaultAllocator s_Allocator;
    static physx::PxFoundation *s_Foundation;
    static physx::PxPvd *s_PVD;
    static physx::PxPhysics *s_Physics;
    static physx::PxCooking *s_CookingFactory;
    static physx::PxOverlapHit s_OverlapBuffer[OVERLAP_MAX_COLLIDERS];

    static ContactListener s_ContactListener;

    void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char *message, const char *file,
                                           int line) {
        const char *errorMessage = NULL;

        switch (code) {
        case physx::PxErrorCode::eNO_ERROR: errorMessage = "No Error"; break;
        case physx::PxErrorCode::eDEBUG_INFO: errorMessage = "Info"; break;
        case physx::PxErrorCode::eDEBUG_WARNING: errorMessage = "Warning"; break;
        case physx::PxErrorCode::eINVALID_PARAMETER: errorMessage = "Invalid Parameter"; break;
        case physx::PxErrorCode::eINVALID_OPERATION: errorMessage = "Invalid Operation"; break;
        case physx::PxErrorCode::eOUT_OF_MEMORY: errorMessage = "Out Of Memory"; break;
        case physx::PxErrorCode::eINTERNAL_ERROR: errorMessage = "Internal Error"; break;
        case physx::PxErrorCode::eABORT: errorMessage = "Abort"; break;
        case physx::PxErrorCode::ePERF_WARNING: errorMessage = "Performance Warning"; break;
        case physx::PxErrorCode::eMASK_ALL: errorMessage = "Unknown Error"; break;
        }

        switch (code) {
        case physx::PxErrorCode::eNO_ERROR:
        case physx::PxErrorCode::eDEBUG_INFO:
            MND_CORE_INFO("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
            break;
        case physx::PxErrorCode::eDEBUG_WARNING:
        case physx::PxErrorCode::ePERF_WARNING:
            MND_CORE_WARN("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
            break;
        case physx::PxErrorCode::eINVALID_PARAMETER:
        case physx::PxErrorCode::eINVALID_OPERATION:
        case physx::PxErrorCode::eOUT_OF_MEMORY:
        case physx::PxErrorCode::eINTERNAL_ERROR:
            MND_CORE_ERROR("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
            break;
        case physx::PxErrorCode::eABORT:
        case physx::PxErrorCode::eMASK_ALL:
            MND_CORE_FATAL("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
            MND_CORE_ASSERT(false);
            break;
        }
    }

    void ContactListener::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) {
        PX_UNUSED(constraints);
        PX_UNUSED(count);
    }

    void ContactListener::onWake(physx::PxActor **actors, physx::PxU32 count) {
        for (uint32_t i = 0; i < count; i++) {
            physx::PxActor &actor = *actors[i];
            Entity &entity = *(Entity *)actor.userData;

            MND_CORE_INFO("PhysX Actor waking up: ID: {0}, Name: {1}", entity.GetUUID(),
                          entity.GetComponent<TagComponent>().Tag);
        }
    }

    void ContactListener::onSleep(physx::PxActor **actors, physx::PxU32 count) {
        for (uint32_t i = 0; i < count; i++) {
            physx::PxActor &actor = *actors[i];
            Entity &entity = *(Entity *)actor.userData;

            MND_CORE_INFO("PhysX Actor going to sleep: ID: {0}, Name: {1}", entity.GetUUID(),
                          entity.GetComponent<TagComponent>().Tag);
        }
    }

    void ContactListener::onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs,
                                    physx::PxU32 nbPairs) {
        Entity &a = *(Entity *)pairHeader.actors[0]->userData;
        Entity &b = *(Entity *)pairHeader.actors[1]->userData;

        if (pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH) {
            if (ScriptEngine::IsEntityModuleValid(a))
                ScriptEngine::OnCollisionBegin(a);
            if (ScriptEngine::IsEntityModuleValid(b))
                ScriptEngine::OnCollisionBegin(b);
        } else if (pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH) {
            if (ScriptEngine::IsEntityModuleValid(a))
                ScriptEngine::OnCollisionEnd(a);
            if (ScriptEngine::IsEntityModuleValid(b))
                ScriptEngine::OnCollisionEnd(b);
        }
    }

    void ContactListener::onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) {
        Entity &a = *(Entity *)pairs->triggerActor->userData;
        Entity &b = *(Entity *)pairs->otherActor->userData;

        if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            if (ScriptEngine::IsEntityModuleValid(a))
                ScriptEngine::OnTriggerBegin(a);
            if (ScriptEngine::IsEntityModuleValid(b))
                ScriptEngine::OnTriggerBegin(b);
        } else if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            if (ScriptEngine::IsEntityModuleValid(a))
                ScriptEngine::OnTriggerEnd(a);
            if (ScriptEngine::IsEntityModuleValid(b))
                ScriptEngine::OnTriggerEnd(b);
        }
    }

    void ContactListener::onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer,
                                    const physx::PxU32 count) {
        PX_UNUSED(bodyBuffer);
        PX_UNUSED(poseBuffer);
        PX_UNUSED(count);
    }

    static physx::PxBroadPhaseType::Enum MonadoToPhysXBroadphaseType(BroadphaseType type) {
        switch (type) {
        case Monado::BroadphaseType::SweepAndPrune: return physx::PxBroadPhaseType::eSAP;
        case Monado::BroadphaseType::MultiBoxPrune: return physx::PxBroadPhaseType::eMBP;
        case Monado::BroadphaseType::AutomaticBoxPrune: return physx::PxBroadPhaseType::eABP;
        }

        return physx::PxBroadPhaseType::eABP;
    }

    static physx::PxFrictionType::Enum MonadoToPhysXFrictionType(FrictionType type) {
        switch (type) {
        case Monado::FrictionType::Patch: return physx::PxFrictionType::ePATCH;
        case Monado::FrictionType::OneDirectional: return physx::PxFrictionType::eONE_DIRECTIONAL;
        case Monado::FrictionType::TwoDirectional: return physx::PxFrictionType::eTWO_DIRECTIONAL;
        }

        return physx::PxFrictionType::ePATCH;
    }

    physx::PxScene *PXPhysicsWrappers::CreateScene() {
        physx::PxSceneDesc sceneDesc(s_Physics->getTolerancesScale());

        const PhysicsSettings &settings = Physics::GetSettings();

        sceneDesc.gravity = ToPhysXVector(settings.Gravity);
        sceneDesc.broadPhaseType = MonadoToPhysXBroadphaseType(settings.BroadphaseAlgorithm);
        sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
        sceneDesc.filterShader = MonadoFilterShader;
        sceneDesc.simulationEventCallback = &s_ContactListener;
        sceneDesc.frictionType = MonadoToPhysXFrictionType(settings.FrictionModel);

        MND_CORE_ASSERT(sceneDesc.isValid());
        return s_Physics->createScene(sceneDesc);
    }

    void PXPhysicsWrappers::AddBoxCollider(PhysicsActor &actor, const physx::PxMaterial &material) {
        auto &collider = actor.m_Entity.GetComponent<BoxColliderComponent>();
        glm::vec3 size = actor.m_Entity.Transform().Scale;
        glm::vec3 colliderSize = collider.Size;

        if (size.x != 0.0F)
            colliderSize.x *= size.x;
        if (size.y != 0.0F)
            colliderSize.y *= size.y;
        if (size.z != 0.0F)
            colliderSize.z *= size.z;

        physx::PxBoxGeometry boxGeometry =
            physx::PxBoxGeometry(colliderSize.x / 2.0F, colliderSize.y / 2.0F, colliderSize.z / 2.0F);
        physx::PxShape *shape =
            physx::PxRigidActorExt::createExclusiveShape(*actor.m_ActorInternal, boxGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
        shape->setLocalPose(ToPhysXTransform(glm::translate(glm::mat4(1.0F), collider.Offset)));
    }

    void PXPhysicsWrappers::AddSphereCollider(PhysicsActor &actor, const physx::PxMaterial &material) {
        auto &collider = actor.m_Entity.GetComponent<SphereColliderComponent>();

        float colliderRadius = collider.Radius;
        glm::vec3 size = actor.m_Entity.Transform().Scale;
        if (size.x != 0.0F)
            colliderRadius *= size.x;

        physx::PxSphereGeometry sphereGeometry = physx::PxSphereGeometry(colliderRadius);
        physx::PxShape *shape =
            physx::PxRigidActorExt::createExclusiveShape(*actor.m_ActorInternal, sphereGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
    }

    void PXPhysicsWrappers::AddCapsuleCollider(PhysicsActor &actor, const physx::PxMaterial &material) {
        auto &collider = actor.m_Entity.GetComponent<CapsuleColliderComponent>();

        float colliderRadius = collider.Radius;
        float colliderHeight = collider.Height;
        glm::vec3 size = actor.m_Entity.Transform().Scale;
        if (size.x != 0.0F)
            colliderRadius *= (size.x / 2.0F);
        if (size.y != 0.0F)
            colliderHeight *= size.y;

        physx::PxCapsuleGeometry capsuleGeometry = physx::PxCapsuleGeometry(colliderRadius, colliderHeight / 2.0F);
        physx::PxShape *shape =
            physx::PxRigidActorExt::createExclusiveShape(*actor.m_ActorInternal, capsuleGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
        shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))));
    }

    void PXPhysicsWrappers::AddMeshCollider(PhysicsActor &actor, const physx::PxMaterial &material) {
        auto &collider = actor.m_Entity.GetComponent<MeshColliderComponent>();
        glm::vec3 size = actor.m_Entity.Transform().Scale;

        if (collider.IsConvex) {
            std::vector<physx::PxConvexMesh *> meshes = CreateConvexMesh(collider);

            for (auto mesh : meshes) {
                physx::PxConvexMeshGeometry convexGeometry =
                    physx::PxConvexMeshGeometry(mesh, physx::PxMeshScale(ToPhysXVector(size)));
                convexGeometry.meshFlags = physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS;
                physx::PxShape *shape =
                    physx::PxRigidActorExt::createExclusiveShape(*actor.m_ActorInternal, convexGeometry, material);
                shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
                shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
            }
        } else {
            std::vector<physx::PxTriangleMesh *> meshes = CreateTriangleMesh(collider);

            for (auto mesh : meshes) {
                physx::PxTriangleMeshGeometry convexGeometry =
                    physx::PxTriangleMeshGeometry(mesh, physx::PxMeshScale(ToPhysXVector(size)));
                physx::PxShape *shape =
                    physx::PxRigidActorExt::createExclusiveShape(*actor.m_ActorInternal, convexGeometry, material);
                shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
                shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
            }
        }
    }

    std::vector<physx::PxTriangleMesh *> PXPhysicsWrappers::CreateTriangleMesh(MeshColliderComponent &collider,
                                                                               bool invalidateOld) {
        std::vector<physx::PxTriangleMesh *> meshes;

        if (invalidateOld)
            PhysicsMeshSerializer::DeleteIfSerialized(collider.CollisionMesh->GetFilePath());

        if (!PhysicsMeshSerializer::IsSerialized(collider.CollisionMesh->GetFilePath())) {
            const std::vector<Vertex> &vertices = collider.CollisionMesh->GetStaticVertices();
            const std::vector<Index> &indices = collider.CollisionMesh->GetIndices();

            std::vector<glm::vec3> vertexPositions;
            for (const auto &vertex : vertices)
                vertexPositions.push_back(vertex.Position);

            for (const auto &submesh : collider.CollisionMesh->GetSubmeshes()) {
                physx::PxTriangleMeshDesc triangleDesc;
                triangleDesc.points.count = submesh.VertexCount;
                triangleDesc.points.stride = sizeof(glm::vec3);
                triangleDesc.points.data = &vertexPositions[submesh.BaseVertex];
                triangleDesc.triangles.count = submesh.IndexCount / 3;
                triangleDesc.triangles.data = &indices[submesh.BaseIndex / 3];
                triangleDesc.triangles.stride = sizeof(Index);

                physx::PxDefaultMemoryOutputStream buf;
                physx::PxTriangleMeshCookingResult::Enum result;
                if (!s_CookingFactory->cookTriangleMesh(triangleDesc, buf, &result)) {
                    MND_CORE_ERROR("Failed to cook triangle mesh: {0}", submesh.MeshName);
                    continue;
                }

                PhysicsMeshSerializer::SerializeMesh(collider.CollisionMesh->GetFilePath(), buf, submesh.MeshName);
                physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
                meshes.push_back(s_Physics->createTriangleMesh(input));
            }
        } else {
            std::vector<physx::PxDefaultMemoryInputData> serializedMeshes =
                PhysicsMeshSerializer::DeserializeMesh(collider.CollisionMesh->GetFilePath());

            for (auto &meshData : serializedMeshes)
                meshes.push_back(s_Physics->createTriangleMesh(meshData));

            PhysicsMeshSerializer::CleanupDataBuffers();
        }

        if (collider.ProcessedMeshes.size() <= 0) {
            for (auto mesh : meshes) {
                const uint32_t nbVerts = mesh->getNbVertices();
                const physx::PxVec3 *triangleVertices = mesh->getVertices();
                const uint32_t nbTriangles = mesh->getNbTriangles();
                const physx::PxU16 *tris = (const physx::PxU16 *)mesh->getTriangles();

                std::vector<Vertex> vertices;
                std::vector<Index> indices;

                for (uint32_t v = 0; v < nbVerts; v++) {
                    Vertex v1;
                    v1.Position = FromPhysXVector(triangleVertices[v]);
                    vertices.push_back(v1);
                }

                for (uint32_t tri = 0; tri < nbTriangles; tri++) {
                    Index index;
                    index.V1 = tris[3 * tri + 0];
                    index.V2 = tris[3 * tri + 1];
                    index.V3 = tris[3 * tri + 2];
                    indices.push_back(index);
                }

                collider.ProcessedMeshes.push_back(Ref<Mesh>::Create(vertices, indices));
            }
        }

        return meshes;
    }

    std::vector<physx::PxConvexMesh *> PXPhysicsWrappers::CreateConvexMesh(MeshColliderComponent &collider,
                                                                           bool invalidateOld) {
        std::vector<physx::PxConvexMesh *> meshes;

        const physx::PxCookingParams &currentParams = s_CookingFactory->getParams();
        physx::PxCookingParams newParams = currentParams;
        newParams.planeTolerance = 0.0F;
        newParams.meshPreprocessParams =
            physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
        newParams.meshWeldTolerance = 0.01f;
        s_CookingFactory->setParams(newParams);

        if (invalidateOld)
            PhysicsMeshSerializer::DeleteIfSerialized(collider.CollisionMesh->GetFilePath());

        if (!PhysicsMeshSerializer::IsSerialized(collider.CollisionMesh->GetFilePath())) {
            const std::vector<Vertex> &vertices = collider.CollisionMesh->GetStaticVertices();
            const std::vector<Index> &indices = collider.CollisionMesh->GetIndices();
            std::vector<glm::vec3> vertexPositions;
            for (const auto &vertex : vertices)
                vertexPositions.push_back(vertex.Position);

            for (const auto &submesh : collider.CollisionMesh->GetSubmeshes()) {
                physx::PxConvexMeshDesc convexDesc;
                convexDesc.points.count = submesh.VertexCount;
                convexDesc.points.stride = sizeof(glm::vec3);
                convexDesc.points.data = &vertexPositions[submesh.BaseVertex];
                convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX |
                                   physx::PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES |
                                   physx::PxConvexFlag::eSHIFT_VERTICES;

                physx::PxDefaultMemoryOutputStream buf;
                physx::PxConvexMeshCookingResult::Enum result;
                if (!s_CookingFactory->cookConvexMesh(convexDesc, buf, &result)) {
                    MND_CORE_ERROR("Failed to cook convex mesh {0}", submesh.MeshName);
                    continue;
                }

                PhysicsMeshSerializer::SerializeMesh(collider.CollisionMesh->GetFilePath(), buf, submesh.MeshName);
                physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
                meshes.push_back(s_Physics->createConvexMesh(input));
            }
        } else {
            std::vector<physx::PxDefaultMemoryInputData> serializedMeshes =
                PhysicsMeshSerializer::DeserializeMesh(collider.CollisionMesh->GetFilePath());

            for (auto &meshData : serializedMeshes)
                meshes.push_back(s_Physics->createConvexMesh(meshData));

            PhysicsMeshSerializer::CleanupDataBuffers();
        }

        if (collider.ProcessedMeshes.size() <= 0) {
            for (auto mesh : meshes) {
                // Based On:
                // https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/ThirdParty/PhysX3/NvCloth/samples/SampleBase/renderer/ConvexRenderMesh.cpp
                const uint32_t nbPolygons = mesh->getNbPolygons();
                const physx::PxVec3 *convexVertices = mesh->getVertices();
                const physx::PxU8 *convexIndices = mesh->getIndexBuffer();

                uint32_t nbVertices = 0;
                uint32_t nbFaces = 0;

                std::vector<Vertex> collisionVertices;
                std::vector<Index> collisionIndices;
                uint32_t vertCounter = 0;
                uint32_t indexCounter = 0;

                for (uint32_t i = 0; i < nbPolygons; i++) {
                    physx::PxHullPolygon polygon;
                    mesh->getPolygonData(i, polygon);
                    nbVertices += polygon.mNbVerts;
                    nbFaces += (polygon.mNbVerts - 2) * 3;

                    uint32_t vI0 = vertCounter;

                    for (uint32_t vI = 0; vI < polygon.mNbVerts; vI++) {
                        Vertex v;
                        v.Position = FromPhysXVector(convexVertices[convexIndices[polygon.mIndexBase + vI]]);
                        collisionVertices.push_back(v);
                        vertCounter++;
                    }

                    for (uint32_t vI = 1; vI < uint32_t(polygon.mNbVerts) - 1; vI++) {
                        Index index;
                        index.V1 = uint32_t(vI0);
                        index.V2 = uint32_t(vI0 + vI + 1);
                        index.V3 = uint32_t(vI0 + vI);
                        collisionIndices.push_back(index);
                        indexCounter++;
                    }

                    collider.ProcessedMeshes.push_back(Ref<Mesh>::Create(collisionVertices, collisionIndices));
                }
            }
        }

        s_CookingFactory->setParams(currentParams);
        return meshes;
    }

    bool PXPhysicsWrappers::Raycast(const glm::vec3 &origin, const glm::vec3 &direction, float maxDistance,
                                    RaycastHit *hit) {
        physx::PxScene *scene = static_cast<physx::PxScene *>(Physics::GetPhysicsScene());
        physx::PxRaycastBuffer hitInfo;
        bool result =
            scene->raycast(ToPhysXVector(origin), ToPhysXVector(glm::normalize(direction)), maxDistance, hitInfo);

        if (result) {
            Entity &entity = *(Entity *)hitInfo.block.actor->userData;
            hit->EntityID = entity.GetUUID();
            hit->Position = FromPhysXVector(hitInfo.block.position);
            hit->Normal = FromPhysXVector(hitInfo.block.normal);
            hit->Distance = hitInfo.block.distance;
        }

        return result;
    }

    bool PXPhysicsWrappers::OverlapBox(const glm::vec3 &origin, const glm::vec3 &halfSize,
                                       std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS> &buffer,
                                       uint32_t *count) {
        physx::PxScene *scene = static_cast<physx::PxScene *>(Physics::GetPhysicsScene());

        memset(s_OverlapBuffer, 0, sizeof(s_OverlapBuffer));
        physx::PxOverlapBuffer buf(s_OverlapBuffer, OVERLAP_MAX_COLLIDERS);
        physx::PxBoxGeometry geometry = physx::PxBoxGeometry(halfSize.x, halfSize.y, halfSize.z);
        physx::PxTransform pose = ToPhysXTransform(glm::translate(glm::mat4(1.0F), origin));

        bool result = scene->overlap(geometry, pose, buf);

        if (result) {
            uint32_t bodyCount = buf.nbTouches >= OVERLAP_MAX_COLLIDERS ? OVERLAP_MAX_COLLIDERS : buf.nbTouches;
            memcpy(buffer.data(), buf.touches, bodyCount * sizeof(physx::PxOverlapHit));
            *count = bodyCount;
        }

        return result;
    }

    bool PXPhysicsWrappers::OverlapCapsule(const glm::vec3 &origin, float radius, float halfHeight,
                                           std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS> &buffer,
                                           uint32_t *count) {
        physx::PxScene *scene = static_cast<physx::PxScene *>(Physics::GetPhysicsScene());

        memset(s_OverlapBuffer, 0, sizeof(s_OverlapBuffer));
        physx::PxOverlapBuffer buf(s_OverlapBuffer, OVERLAP_MAX_COLLIDERS);
        physx::PxCapsuleGeometry geometry = physx::PxCapsuleGeometry(radius, halfHeight);
        physx::PxTransform pose = ToPhysXTransform(glm::translate(glm::mat4(1.0F), origin));

        bool result = scene->overlap(geometry, pose, buf);

        if (result) {
            uint32_t bodyCount = buf.nbTouches >= OVERLAP_MAX_COLLIDERS ? OVERLAP_MAX_COLLIDERS : buf.nbTouches;
            memcpy(buffer.data(), buf.touches, bodyCount * sizeof(physx::PxOverlapHit));
            *count = bodyCount;
        }

        return result;
    }

    bool PXPhysicsWrappers::OverlapSphere(const glm::vec3 &origin, float radius,
                                          std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS> &buffer,
                                          uint32_t *count) {
        physx::PxScene *scene = static_cast<physx::PxScene *>(Physics::GetPhysicsScene());

        memset(s_OverlapBuffer, 0, sizeof(s_OverlapBuffer));
        physx::PxOverlapBuffer buf(s_OverlapBuffer, OVERLAP_MAX_COLLIDERS);
        physx::PxSphereGeometry geometry = physx::PxSphereGeometry(radius);
        physx::PxTransform pose = ToPhysXTransform(glm::translate(glm::mat4(1.0F), origin));

        bool result = scene->overlap(geometry, pose, buf);

        if (result) {
            uint32_t bodyCount = buf.nbTouches >= OVERLAP_MAX_COLLIDERS ? OVERLAP_MAX_COLLIDERS : buf.nbTouches;
            memcpy(buffer.data(), buf.touches, bodyCount * sizeof(physx::PxOverlapHit));
            *count = bodyCount;
        }

        return result;
    }

    physx::PxPhysics &PXPhysicsWrappers::GetPhysics() { return *s_Physics; }

    void PXPhysicsWrappers::Initialize() {
        MND_CORE_ASSERT(!s_Foundation, "PXPhysicsWrappers::Initializer shouldn't be called more than once!");

        s_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);
        MND_CORE_ASSERT(s_Foundation, "PxCreateFoundation Failed!");

        s_PVD = PxCreatePvd(*s_Foundation);
        if (s_PVD) {
            physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
            s_PVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        }

        physx::PxTolerancesScale scale = physx::PxTolerancesScale();
        scale.length = 10;
        s_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_Foundation, scale, true, s_PVD);
        MND_CORE_ASSERT(s_Physics, "PxCreatePhysics Failed!");

        s_CookingFactory = PxCreateCooking(PX_PHYSICS_VERSION, *s_Foundation, s_Physics->getTolerancesScale());
        MND_CORE_ASSERT(s_CookingFactory, "PxCreatePhysics Failed!");

        PxSetAssertHandler(s_AssertHandler);
    }

    void PXPhysicsWrappers::Shutdown() {
        s_CookingFactory->release();
        s_Physics->release();
        s_Foundation->release();
    }

    physx::PxAllocatorCallback &PXPhysicsWrappers::GetAllocator() { return s_Allocator; }

    void PhysicsAssertHandler::operator()(const char *exp, const char *file, int line, bool &ignore) {
        MND_CORE_ERROR("[PhysX Error]: {0}:{1} - {2}", file, line, exp);

#if 0
		MND_CORE_ASSERT(false);
#endif
    }

} // namespace Monado

#include "monado/physics/pxPhysicsWrappers.h"
#include "monado/physics/physics.h"
#include "monado/physics/physicsLayer.h"
#include "monado/script/scriptEngine.h"

#include "glm/gtx/rotate_vector.hpp"

namespace Monado {

    static PhysicsErrorCallback s_ErrorCallback;
    static physx::PxDefaultAllocator s_Allocator;
    static physx::PxFoundation *s_Foundation;
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

    physx::PxRigidActor *PXPhysicsWrappers::CreateActor(const RigidBodyComponent &rigidbody,
                                                        const TransformComponent &transform) {
        physx::PxRigidActor *actor = nullptr;

        const PhysicsSettings &settings = Physics::GetSettings();

        if (rigidbody.BodyType == RigidBodyComponent::Type::Static) {
            actor = s_Physics->createRigidStatic(ToPhysXTransform(transform));
        } else if (rigidbody.BodyType == RigidBodyComponent::Type::Dynamic) {
            physx::PxRigidDynamic *dynamicActor = s_Physics->createRigidDynamic(ToPhysXTransform(transform));

            dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidbody.IsKinematic);

            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X,
                                                  rigidbody.LockPositionX);
            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,
                                                  rigidbody.LockPositionY);
            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z,
                                                  rigidbody.LockPositionZ);
            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X,
                                                  rigidbody.LockRotationX);
            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
                                                  rigidbody.LockRotationY);
            dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z,
                                                  rigidbody.LockRotationZ);

            dynamicActor->setSolverIterationCounts(settings.SolverIterations, settings.SolverVelocityIterations);

            physx::PxRigidBodyExt::updateMassAndInertia(*dynamicActor, rigidbody.Mass);
            actor = dynamicActor;
        }

        return actor;
    }

    void PXPhysicsWrappers::SetCollisionFilters(const physx::PxRigidActor &actor, uint32_t physicsLayer) {
        const PhysicsLayer &layerInfo = PhysicsLayerManager::GetLayer(physicsLayer);

        if (layerInfo.CollidesWith == 0)
            return;

        physx::PxFilterData filterData;
        filterData.word0 = layerInfo.BitValue;
        filterData.word1 = layerInfo.CollidesWith;

        const physx::PxU32 numShapes = actor.getNbShapes();

        physx::PxShape **shapes =
            (physx::PxShape **)s_Allocator.allocate(sizeof(physx::PxShape *) * numShapes, "", "", 0);
        actor.getShapes(shapes, numShapes);

        for (physx::PxU32 i = 0; i < numShapes; i++)
            shapes[i]->setSimulationFilterData(filterData);

        s_Allocator.deallocate(shapes);
    }

    void PXPhysicsWrappers::AddBoxCollider(physx::PxRigidActor &actor, const physx::PxMaterial &material,
                                           const BoxColliderComponent &collider, const glm::vec3 &size) {
        glm::vec3 colliderSize = collider.Size;

        if (size.x != 0.0F)
            colliderSize.x *= size.x;
        if (size.y != 0.0F)
            colliderSize.y *= size.y;
        if (size.z != 0.0F)
            colliderSize.z *= size.z;

        physx::PxBoxGeometry boxGeometry =
            physx::PxBoxGeometry(colliderSize.x / 2.0F, colliderSize.y / 2.0F, colliderSize.z / 2.0F);
        physx::PxShape *shape = physx::PxRigidActorExt::createExclusiveShape(actor, boxGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
        shape->setLocalPose(ToPhysXTransform(glm::translate(glm::mat4(1.0F), collider.Offset)));
    }

    void PXPhysicsWrappers::AddSphereCollider(physx::PxRigidActor &actor, const physx::PxMaterial &material,
                                              const SphereColliderComponent &collider, const glm::vec3 &size) {
        float colliderRadius = collider.Radius;

        if (size.x != 0.0F)
            colliderRadius *= size.x;

        physx::PxSphereGeometry sphereGeometry = physx::PxSphereGeometry(colliderRadius);
        physx::PxShape *shape = physx::PxRigidActorExt::createExclusiveShape(actor, sphereGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
    }

    void PXPhysicsWrappers::AddCapsuleCollider(physx::PxRigidActor &actor, const physx::PxMaterial &material,
                                               const CapsuleColliderComponent &collider, const glm::vec3 &size) {
        float colliderRadius = collider.Radius;
        float colliderHeight = collider.Height;

        if (size.x != 0.0F)
            colliderRadius *= (size.x / 2.0F);
        if (size.y != 0.0F)
            colliderHeight *= size.y;

        physx::PxCapsuleGeometry capsuleGeometry = physx::PxCapsuleGeometry(colliderRadius, colliderHeight / 2.0F);
        physx::PxShape *shape = physx::PxRigidActorExt::createExclusiveShape(actor, capsuleGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
        shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))));
    }

    void PXPhysicsWrappers::AddMeshCollider(physx::PxRigidActor &actor, const physx::PxMaterial &material,
                                            MeshColliderComponent &collider, const glm::vec3 &size) {
        physx::PxConvexMeshGeometry convexGeometry = physx::PxConvexMeshGeometry(CreateConvexMesh(collider));
        convexGeometry.meshFlags = physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS;
        physx::PxShape *shape = physx::PxRigidActorExt::createExclusiveShape(actor, convexGeometry, material);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !collider.IsTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, collider.IsTrigger);
    }

    physx::PxConvexMesh *PXPhysicsWrappers::CreateConvexMesh(MeshColliderComponent &collider) {
        physx::PxConvexMesh *mesh = nullptr;
        if (!ConvexMeshSerializer::IsSerialized(collider.CollisionMesh->GetFilePath())) {
            std::vector<Vertex> vertices = collider.CollisionMesh->GetStaticVertices();

            physx::PxConvexMeshDesc convexDesc;
            convexDesc.points.count = vertices.size();
            convexDesc.points.stride = sizeof(Vertex);
            convexDesc.points.data = vertices.data();
            convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

            physx::PxDefaultMemoryOutputStream buf;
            physx::PxConvexMeshCookingResult::Enum result;
            if (!s_CookingFactory->cookConvexMesh(convexDesc, buf, &result))
                MND_CORE_ASSERT(false);

            ConvexMeshSerializer::SerializeMesh(collider.CollisionMesh->GetFilePath(), buf);
            physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
            mesh = s_Physics->createConvexMesh(input);
        } else {
            physx::PxDefaultMemoryInputData input =
                ConvexMeshSerializer::DeserializeMesh(collider.CollisionMesh->GetFilePath());
            mesh = s_Physics->createConvexMesh(input);
        }

        // TODO: This doesn't really belong here since this generates the debug mesh used for the editor
        if (!collider.ProcessedMesh) {
            // Based On:
            // https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/ThirdParty/PhysX3/NvCloth/samples/SampleBase/renderer/ConvexRenderMesh.cpp

            const uint32_t nbPolygons = mesh->getNbPolygons();
            const physx::PxVec3 *convexVertices = mesh->getVertices();
            const physx::PxU8 *convexIndices = mesh->getIndexBuffer();

            uint32_t nbVertices = 0;
            uint32_t nbFaces = 0;

            for (uint32_t i = 0; i < nbPolygons; i++) {
                physx::PxHullPolygon polygon;
                mesh->getPolygonData(i, polygon);
                nbVertices += polygon.mNbVerts;
                nbFaces += (polygon.mNbVerts - 2) * 3;
            }

            std::vector<Vertex> collisionVertices;
            std::vector<Index> collisionIndices;

            collisionVertices.resize(nbVertices);
            collisionIndices.resize(nbFaces / 3);

            uint32_t vertCounter = 0;
            uint32_t indexCounter = 0;
            for (uint32_t i = 0; i < nbPolygons; i++) {
                physx::PxHullPolygon polygon;
                mesh->getPolygonData(i, polygon);

                uint32_t vI0 = vertCounter;
                for (uint32_t vI = 0; vI < polygon.mNbVerts; vI++) {
                    collisionVertices[vertCounter].Position =
                        glm::rotate(FromPhysXVector(convexVertices[convexIndices[polygon.mIndexBase + vI]]),
                                    glm::radians(90.0F), { 1, 0, 0 });
                    vertCounter++;
                }

                for (uint32_t vI = 1; vI < uint32_t(polygon.mNbVerts) - 1; vI++) {
                    collisionIndices[indexCounter].V1 = uint32_t(vI0);
                    collisionIndices[indexCounter].V2 = uint32_t(vI0 + vI + 1);
                    collisionIndices[indexCounter].V3 = uint32_t(vI0 + vI);
                    indexCounter++;
                }
            }

            collider.ProcessedMesh = Ref<Mesh>::Create(collisionVertices, collisionIndices);
        }

        return mesh;
    }

    physx::PxMaterial *PXPhysicsWrappers::CreateMaterial(const PhysicsMaterialComponent &material) {
        return s_Physics->createMaterial(material.StaticFriction, material.DynamicFriction, material.Bounciness);
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

    void PXPhysicsWrappers::Initialize() {
        MND_CORE_ASSERT(!s_Foundation, "PXPhysicsWrappers::Initializer shouldn't be called more than once!");

        s_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);
        MND_CORE_ASSERT(s_Foundation, "PxCreateFoundation Failed!");

        s_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_Foundation, physx::PxTolerancesScale(), true);
        MND_CORE_ASSERT(s_Physics, "PxCreatePhysics Failed!");

        s_CookingFactory = PxCreateCooking(PX_PHYSICS_VERSION, *s_Foundation, s_Physics->getTolerancesScale());
        MND_CORE_ASSERT(s_CookingFactory, "PxCreatePhysics Failed!");
    }

    void PXPhysicsWrappers::Shutdown() {
        s_CookingFactory->release();
        s_Physics->release();
        s_Foundation->release();
    }
} // namespace Monado

#include "monado/physics/physicsUtil.h"
#include "monado/physics/physicsLayer.h"

namespace Monado {

    physx::PxTransform ToPhysXTransform(const Transform &transform) {
        physx::PxQuat r = ToPhysXQuat(glm::normalize(glm::quat(glm::radians(transform.GetRotation()))));
        physx::PxVec3 p = ToPhysXVector(transform.GetTranslation());
        return physx::PxTransform(p, r);
    }

    physx::PxTransform ToPhysXTransform(const glm::mat4 &transform) {
        physx::PxQuat r = ToPhysXQuat(glm::normalize(glm::toQuat(transform)));
        physx::PxVec3 p = ToPhysXVector(glm::vec3(transform[3]));
        return physx::PxTransform(p, r);
    }

    physx::PxMat44 ToPhysXMatrix(const glm::mat4 &matrix) { return *(physx::PxMat44 *)&matrix; }

    physx::PxVec3 ToPhysXVector(const glm::vec3 &vector) { return *(physx::PxVec3 *)&vector; }

    physx::PxVec4 ToPhysXVector(const glm::vec4 &vector) { return *(physx::PxVec4 *)&vector; }

    physx::PxQuat ToPhysXQuat(const glm::quat &quat) { return *(physx::PxQuat *)&quat; }

    glm::mat4 FromPhysXTransform(const physx::PxTransform &transform) {
        glm::quat rotation = FromPhysXQuat(transform.q);
        glm::vec3 position = FromPhysXVector(transform.p);
        return glm::translate(glm::mat4(1.0F), position) * glm::toMat4(rotation);
    }

    glm::mat4 FromPhysXMatrix(const physx::PxMat44 &matrix) { return *(glm::mat4 *)&matrix; }

    glm::vec3 FromPhysXVector(const physx::PxVec3 &vector) { return *(glm::vec3 *)&vector; }

    glm::vec4 FromPhysXVector(const physx::PxVec4 &vector) { return *(glm::vec4 *)&vector; }

    glm::quat FromPhysXQuat(const physx::PxQuat &quat) { return *(glm::quat *)&quat; }

    physx::PxFilterFlags MonadoFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                           physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                           physx::PxPairFlags &pairFlags, const void *constantBlock,
                                           physx::PxU32 constantBlockSize) {
        if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) {
            pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
            return physx::PxFilterFlag::eDEFAULT;
        }

        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

        if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1)) {
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
            return physx::PxFilterFlag::eDEFAULT;
        }

        return physx::PxFilterFlag::eSUPPRESS;
    }

} // namespace Monado

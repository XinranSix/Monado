
#include "monado/renderer/camera.h"

#include "monado/core/input.h"
#include "monado/core/log.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
#include "glm/gtc/quaternion.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#define M_PI 3.14159f

namespace Monado {

    Camera::Camera(const glm::mat4 &projectionMatrix) : m_ProjectionMatrix(projectionMatrix) {}

} // namespace Monado

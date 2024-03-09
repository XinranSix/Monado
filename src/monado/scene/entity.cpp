#include "monado/scene/entity.h"

namespace Monado {
    Entity::Entity(entt::entity handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene) {}
} // namespace Monado

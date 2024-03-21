#include "monado/scene/entity.h"

namespace Monado {

    Entity::Entity(const std::string &name) : m_Name(name), m_Transform(1.0f) {}

    Entity::~Entity() {}

} // namespace Monado

#include "monado/core/layer.h"
#include <string>

namespace Monado {
    
    Layer::Layer(const std::string &debugName) : m_DebugName { debugName } {}

    Layer::~Layer() {}
} // namespace Monado

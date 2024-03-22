#pragma once

#include "base.h"
#include <xhash>

namespace Monado {

    // "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
    // used to "uniquely" identify information. In Monado, even though we use the term
    // GUID and UUID, at the moment we're simply using a randomly generated 64-bit
    // integer, as the possibility of a clash is low enough for now.
    // This may change in the future.
    class UUID {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID &other);

        operator uint64_t() { return m_UUID; }
        operator const uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID;
    };

} // namespace Monado

namespace std {

    template <>
    struct hash<Monado::UUID> {
        std::size_t operator()(const Monado::UUID &uuid) const { return hash<uint64_t>()((uint64_t)uuid); }
    };
} // namespace std

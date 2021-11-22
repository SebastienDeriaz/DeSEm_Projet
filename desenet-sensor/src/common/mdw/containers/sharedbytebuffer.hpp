#pragma once
#include "SharedBuffer"
#include <cstdint>

namespace hei {
    /**
     * @brief Shared buffer of bytes.
     */
    typedef SharedBuffer<std::uint8_t> SharedByteBuffer;
}

#ifndef BONSAI_UTILS_HPP
#define BONSAI_UTILS_HPP

#include <cstdint>
#include <cstring>

constexpr uint32_t bytes_to_u32(const uint8_t *bytes) {
    // TODO: check if bytes is nullptr, add a test for that
    uint8_t buffer[4] = {0};
    memcpy(buffer, bytes, 4);   // FIXME: why copy here? why not just use the bytes as it is?
    uint32_t result = buffer[3] << 24 | //
                      buffer[2] << 16 | //
                      buffer[1] << 8 |  //
                      buffer[0];
    return result;
}


constexpr uint32_t reverse_addr(uint32_t value) {
    uint32_t result = 0;
    result |= ((value & 0xFF) << 24);
    result |= (((value >> 8) & 0xFF) << 16);
    result |= (((value >> 16) & 0xFF) << 8);
    result |= ((value >> 24) & 0xFF);
    return result;
}

#endif
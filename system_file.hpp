#ifndef BONSAI_SYSTEM_FILE_HPP
#define BONSAI_SYSTEM_FILE_HPP

#include <cstdint>

// TODO: add a comment for description for each field, function in this struct
struct system_file_data_t {
    uint8_t _fsa[4];
    // TODO: can this function be made constexpr? constexpr uint32_t fsa() const;
    uint32_t fsa(void) {
        return (_fsa[3] << 24) | (_fsa[2] << 16) | (_fsa[1] << 8) | (_fsa[0]); //
    }
};

#endif
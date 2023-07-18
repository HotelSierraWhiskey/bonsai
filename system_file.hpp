#ifndef BONSAI_SYSTEM_FILE_HPP
#define BONSAI_SYSTEM_FILE_HPP

#include "includes.hpp"

typedef struct {
    uint8_t _fsa[4];
    uint32_t fsa(void) {
        return (_fsa[3] << 24) | (_fsa[2] << 16) | (_fsa[1] << 8) | (_fsa[0]); //
    }
} system_file_data_t;

#endif
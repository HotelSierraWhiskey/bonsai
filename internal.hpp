#ifndef BONSAI_INTERNAL_HPP
#define BONSAI_INTERNAL_HPP

#include "config.hpp"

#define PAGE_SIZE 0x40
#define PAGES_PER_ROW 0x04
#define ROW_SIZE (PAGES_PER_ROW * PAGE_SIZE)
#define BYTES_PER_ROW 0x20

#ifndef BONSAI_MEMORY_START
    #error BONSAI_MEMORY_START not defined.
#endif
#if (BONSAI_MEMORY_START % ROW_SIZE)
    #error BONSAI_MEMORY_START is not a row boundary address
#endif

#ifndef BONSAI_MEMORY_END
    #error BONSAI_MEMORY_END not defined.
#endif
#if (BONSAI_MEMORY_END % ROW_SIZE)
    #error BONSAI_MEMORY_END is not a row boundary address
#endif

#define SYSTEM_FILE_ADDRESS BONSAI_MEMORY_START
#define ROOT_DIRECTORY_ADDRESS (SYSTEM_FILE_ADDRESS + ROW_SIZE)

#define PART_TABLE_SIZE 71

typedef struct {
    char name[16];
    uint32_t memory;
    uint32_t variant;
    uint32_t devsel;
} part_t;

extern const std::array<part_t, PART_TABLE_SIZE> part_table;

#endif

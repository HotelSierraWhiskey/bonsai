#ifndef BONSAI_BONSAI_HPP
#define BONSAI_BONSAI_HPP

#include "file.hpp"
#include "internal.hpp"
#include "nvmctrl.hpp"

typedef struct {
    uint32_t free_space_address;
} system_properties_row_t;

class Bonsai {
  public:
    part_t part;
    system_properties_row_t system;
    Nvmc nvm;
    uint32_t fsa;
    Bonsai(void);
    void falloc(file_t *file);
    void create_file(std::string path);
};

#endif
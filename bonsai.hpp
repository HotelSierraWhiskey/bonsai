#ifndef BONSAI_BONSAI_HPP
#define BONSAI_BONSAI_HPP

#include "file.hpp"
#include "internal.hpp"
#include "nvmctrl.hpp"

typedef struct {
    uint32_t free_space_address;
} system_properties_row_t;

class Bonsai {
    // private:
  public:
    system_properties_row_t system;
    Nvmc nvm;
    part_t part;
    void update_free_space_address(uint32_t address) noexcept;

    Bonsai(void);
    part_t get_part(void) noexcept;
    void erase(void) noexcept;
    uint32_t read_free_space_address(void) noexcept;
    void fputf(file_t &file) noexcept;
    file_t fgetf(uint32_t address);
    void create_file(std::string path) noexcept;
};

#endif
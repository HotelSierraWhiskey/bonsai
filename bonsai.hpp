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

    Bonsai(void);
    part_t get_part(void);

    /* Erases entire Bonsai flash partition */
    void erase(void);

    /* Read FSA from flash */
    uint32_t read_free_space_address(void);

    /* Writes new FSA to flash */
    void update_free_space_address(const uint32_t address);

    /* Write a file to flash (updates FSA) */
    void put(file_t &file);
    void put(file_t &file, uint32_t address);
    void put_blank_file(const std::string name, uint32_t parent_address, uint32_t address);

    /* Return file_t from address */
    file_t get(const uint32_t address);

    /* deletes the file at address */
    void del(uint32_t address);

    /* moves file data from flash address src to dest */
    void mov(const uint32_t dest, const uint32_t src);
};

#endif
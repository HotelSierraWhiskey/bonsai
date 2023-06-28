#ifndef BONSAI_BONSAI_HPP
#define BONSAI_BONSAI_HPP

#include "file.hpp"
#include "internal.hpp"
#include "nvmctrl.hpp"

typedef struct {
    uint32_t free_space_address;
} system_properties_row_t;

class Bonsai {
  private:
    system_properties_row_t system;
    Nvmc nvm;
    part_t part;

  public:
    Bonsai(void);
    part_t get_part(void);

    /* erases entire Bonsai flash partition */
    void erase(void);

    /* reads FSA from flash */
    uint32_t read_free_space_address(void);

    /* writes new FSA to flash */
    void update_free_space_address(const uint32_t address);

    /* writes a file to flash (updates FSA) */
    void put(file_t &file);
    void put(file_t &file, uint32_t address);
    void put_blank_file(const std::string name, uint32_t parent_address, uint32_t address);

    /* returns file_t from address */
    file_t get(const uint32_t address);

    /* deletes the file at address */
    void del(const uint32_t address);

    /* moves file data from flash address src to dest */
    void mov(const uint32_t dest, const uint32_t src);

    /* basic file editing methods */
    void edit_file_handle(const uint32_t address, const std::string handle);
    void edit_file_data(const uint32_t address, const std::string data);
    void edit_file_parent_addr(const uint32_t address, const uint32_t parent_addr);
    void add_child_addr(const uint32_t address, uint32_t child_addr);
    void remove_child_addr(const uint32_t address, const uint32_t child_addr);
};

#endif
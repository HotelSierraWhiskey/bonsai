#ifndef BONSAI_BONSAI_HPP
#define BONSAI_BONSAI_HPP

#include "file.hpp"
#include "internal.hpp"
#include "nvmctrl.hpp"
#include "system_file.hpp"

class Bonsai {
  private:
    Nvmc nvm;
    part_t part;
    void init(void);

  public:
    uint32_t fsa;

    /* bonsai.cpp */
    Bonsai(void);
    part_t get_part(void);
    system_file_data_t *get_sfd(void);
    void erase(void);
    uint32_t read_fsa(void);
    void write_fsa(uint32_t address = 0x00);
    uint32_t update_fsa(uint32_t address, uint32_t increment);

    /* primatives.cpp */
    void put(file_t &file);
    void put(file_t &file, uint32_t address);
    void put_blank_file(const std::string name, uint32_t parent_address = ROOT_DIRECTORY_ADDRESS, uint32_t address = 0x00);
    file_t get(const uint32_t address);
    void del(const uint32_t address);
    void mov(const uint32_t dest, const uint32_t src);
    uint32_t find(const uint32_t root, const std::string handle);
    uint32_t find(std::string path);

    /* edit.cpp */
    void edit_file_handle(const uint32_t address, const std::string handle);
    void edit_file_handle(std::string path, const std::string handle);
    void edit_file_data(const uint32_t address, const std::string data);
    void edit_file_data(std::string path, const std::string data);
    void edit_file_parent_addr(const uint32_t address, const uint32_t parent_addr);
    void edit_file_parent_addr(std::string path, const uint32_t parent_addr);
    void add_child_addr(const uint32_t address, uint32_t child_addr);
    void add_child_addr(std::string path, uint32_t child_addr);
    void remove_child_addr(const uint32_t address, const uint32_t child_addr);
    void remove_child_addr(std::string path, const uint32_t child_addr);

    /* api.cpp */
    void create_file(std::string path);
    void delete_file(std::string path);
    void delete_subfiles(uint32_t root);
    void move_file(std::string dest, std::string src);
    void read_file(std::string path);
};

#endif
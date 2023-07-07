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

  public:
    uint32_t fsa;
    Bonsai(void);
    part_t get_part(void);

    /* retrieves System File data */
    system_file_data_t *get_sfd(void);

    /* erases entire Bonsai flash partition */
    void erase(void);

    /* reads FSA from flash */
    uint32_t read_fsa(void);

    /* writes new FSA to flash (calls put)*/
    void write_fsa(uint32_t address = 0x00);

    /* updates fsa member */
    uint32_t update_fsa(uint32_t address, uint32_t increment);

    /* writes a file to flash (updates FSA) */
    void put(file_t &file);
    void put(file_t &file, uint32_t address);
    void put_blank_file(const std::string name, uint32_t parent_address = ROOT_DIRECTORY_ADDRESS,
                        uint32_t address = 0x00);

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

    uint32_t find(const uint32_t root, const std::string handle);

    /* Top level API */
    void create_file(std::string path);
    void delete_file(const std::string path);
    void move_file(const std::string dest, const std::string src);
    void read_file(const std::string path);
};

#endif
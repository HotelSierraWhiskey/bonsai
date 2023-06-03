#include "bonsai/bonsai.hpp"
#include "bonsai/file.hpp"

#define DSU_DID_REGISTER_BASE_ADDRESS (0x41002000 + 0x18)
#define VARIANT_MASK 0xFFFFF
#define VARIANT_POS 0x0C
#define DEVSEL_MASK 0xFF

#define U32_FLASH_RESET_VALUE 0xFFFFFFFF

Bonsai::Bonsai(void) {
    uint32_t *device_id = (uint32_t *)DSU_DID_REGISTER_BASE_ADDRESS;
    uint32_t variant = ((*device_id) & (VARIANT_MASK << VARIANT_POS)) >> VARIANT_POS;
    uint32_t devsel = (*device_id) & (DEVSEL_MASK);

    for (const auto &entry : part_table) {
        if (entry.variant == variant && entry.devsel == devsel) {
            part = entry;
            break;
        }
    }
    uint32_t *sp_ptr = (uint32_t *)SYSTEM_PROPERTIES_ROW_ADDRESS;
    if (*sp_ptr == U32_FLASH_RESET_VALUE) {
        system.free_space_address = ROOT_DIRECTORY_ADDRESS;
    } else {
        system.free_space_address = *sp_ptr;
    }
}

#include "globals.hpp"

void Bonsai::falloc(file_t *file) {
    uint32_t fsize = file->size();
    uint8_t padding = fsize < 0x20 ? 0x20 - (fsize % 0x20) : fsize % 0x20;
    uint32_t total_size = fsize + padding;
    uint8_t buffer[total_size];
    file->to_buffer(buffer);

    uint32_t row_index = 0;
    for (uint8_t i = 0; i < total_size / BYTES_PER_ROW; i++) {
        uint8_t row_buffer[BYTES_PER_ROW];
        for (uint8_t j = 0; j < BYTES_PER_ROW; j++) {
            row_buffer[j] = buffer[row_index++];
        }
        nvm.erase_row(system.free_space_address);
        nvm.write(system.free_space_address, row_buffer, BYTES_PER_ROW);
        system.free_space_address += BYTES_PER_ROW;
    }
}

void Bonsai::create_file(std::string path) {
    if (path.back() != '/') {
        path += "/";
    }

    uint32_t pos = 0;
    std::string token;

    uint8_t i = 0;
    std::array<uint8_t, 16> indexes;

    while ((pos = path.find("/")) != std::string::npos) {
        indexes[i++] = pos;
        token = path.substr(0, pos);
        path.erase(0, pos + 1);
    }
    std::string fname = path.substr(0, pos);

    file_t file = {.handle_size = (uint8_t)fname.length(),
                   .data_size = 0,
                   .parent_addr = 0,
                   .num_child_addrs = 0,
                   .handle = (uint8_t *)fname.c_str(),
                   .data = nullptr,
                   .child_addrs = nullptr};

    // for (uint8_t i = 0; i < indexes.size(); i++) {
    // debug.printf("i = %u\r\n", i);
    // debug.printf("%u\r\n", indexes[i - 1]);
    // debug.printf("%u\r\n", indexes[i - 2]);

    debug.printf("substr");

    // }

    // for (uint8_t i = 0; i < fname.length(); i++) {
    //     debug.printf("%u\r\n", file.handle[i]);
    // }
    // nvm.write(system.free_space_address, );
}
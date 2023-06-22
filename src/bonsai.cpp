#include "bonsai/bonsai.hpp"
#include "bonsai/file.hpp"
#include "globals.hpp"

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

    system.free_space_address = read_free_space_address();
    if (system.free_space_address == U32_FLASH_RESET_VALUE) {
        update_free_space_address(ROOT_DIRECTORY_ADDRESS);
    }
}

part_t Bonsai::get_part(void) {
    return part; //
}

uint32_t Bonsai::read_free_space_address(void) {
    uint32_t *p = (uint32_t *)SYSTEM_PROPERTIES_ROW_ADDRESS;
    uint8_t buffer[4];
    uint8_t *bp = buffer;
    memcpy(buffer, p, 4);
    uint32_t address = bp[0] << 24 | bp[1] << 16 | bp[2] << 8 | bp[3];
    return address;
}

void Bonsai::erase(void) {
    uint32_t address = SYSTEM_PROPERTIES_ROW_ADDRESS;
    while (address < BONSAI_MEMORY_END) {
        nvm.erase_row(address);
        address += ROW_SIZE;
    }
    update_free_space_address(ROOT_DIRECTORY_ADDRESS);
}

void Bonsai::update_free_space_address(const uint32_t address) {
    uint32_t *p = (uint32_t *)SYSTEM_PROPERTIES_ROW_ADDRESS;

    uint8_t buffer[PAGE_SIZE];
    memset(buffer, 0, PAGE_SIZE);
    memcpy(buffer, p, PAGE_SIZE);

    buffer[0] = (address & (0xFF << 24)) >> 24;
    buffer[1] = (address & (0xFF << 16)) >> 16;
    buffer[2] = (address & (0xFF << 8)) >> 8;
    buffer[3] = (address & (0xFF));

    nvm.write_page(SYSTEM_PROPERTIES_ROW_ADDRESS - PAGE_SIZE, buffer);
    system.free_space_address = address;
}

void Bonsai::put(file_t &file) {
    uint32_t fsize = file.size();
    uint8_t padding = ROW_SIZE - (fsize % ROW_SIZE);
    uint32_t total_size = fsize + padding;
    uint8_t buffer[total_size];
    memset(buffer, 0, total_size);
    file.to_buffer(buffer);

    uint32_t buffer_index = 0;
    for (uint8_t i = 0; i < total_size / ROW_SIZE; i++) {
        nvm.erase_row(system.free_space_address);
        for (uint8_t j = 0; j < 4; j++) {
            uint8_t page_buffer[PAGE_SIZE];
            memcpy(page_buffer, buffer + buffer_index, PAGE_SIZE);
            nvm.write_page(system.free_space_address - PAGE_SIZE, page_buffer);

            buffer_index += PAGE_SIZE;
            update_free_space_address(system.free_space_address + PAGE_SIZE);
        }
    }
}

void Bonsai::put(file_t &file, uint32_t address) {
    uint32_t saved = system.free_space_address;
    system.free_space_address = address;
    put(file);
    system.free_space_address = saved;
}

void Bonsai::put_blank_file(const std::string name, uint32_t parent_address, uint32_t address) {
    file_t file = {
        .handle_size = (uint8_t)name.size(),
        .data_size = 0,
        .parent_addr = parent_address,
        .num_child_addrs = 0,
        .handle = (uint8_t *)name.c_str(),
        .data = (uint8_t *)nullptr,
        .child_addrs = nullptr,
    };
    put(file, address);
}

file_t Bonsai::get(const uint32_t address) {
    uint8_t *p = (uint8_t *)address;
    uint8_t handle_size = *p++;
    uint16_t data_size = *p++ << 8 | *p++;

    uint32_t parent_addr = *p++ << 24 | //
                           *p++ << 16 | //
                           *p++ << 8 |  //
                           *p++;        //

    uint8_t num_child_addrs = *p++;

    uint32_t offset = sizeof(handle_size) + sizeof(data_size) + sizeof(parent_addr) + sizeof(num_child_addrs);

    uint8_t *handle_start_addr = (uint8_t *)(address + offset);
    uint8_t *data_start_addr = (uint8_t *)(address + offset + handle_size);
    uint32_t *child_addrs_start_addr = (uint32_t *)(address + offset + handle_size + data_size);

    file_t file = {.handle_size = (uint8_t)handle_size,
                   .data_size = (uint16_t)data_size,
                   .parent_addr = parent_addr,
                   .num_child_addrs = num_child_addrs,
                   .handle = (uint8_t *)handle_start_addr,
                   .data = (uint8_t *)data_start_addr,
                   .child_addrs = (uint32_t *)child_addrs_start_addr};

    return file;
}

void Bonsai::del(uint32_t address) {
    auto file = get(address);
    uint32_t fsize = file.size();
    uint8_t padding = ROW_SIZE - (fsize % ROW_SIZE);
    uint32_t total_size = fsize + padding;
    for (uint8_t i = 0; i < total_size / ROW_SIZE; i++) {
        nvm.erase_row(address);
        address += PAGE_SIZE;
    }
}

void Bonsai::mov(const uint32_t dest, const uint32_t src) {
    auto file = get(src);
    put(file, dest);
    del(src);
}

// void Bonsai::create_file(std::string path) {
//     if (path.back() != '/') {
//         path += "/";
//     }

//     uint32_t pos = 0;
//     std::string token;

//     uint8_t i = 0;
//     std::array<uint8_t, 16> indexes;

//     while ((pos = path.find("/")) != std::string::npos) {
//         indexes[i++] = pos;
//         token = path.substr(0, pos);
//         path.erase(0, pos + 1);
//     }
//     std::string fname = path.substr(0, pos);

//     file_t file = {.handle_size = (uint8_t)fname.length(),
//                    .data_size = 0,
//                    .parent_addr = 0,
//                    .num_child_addrs = 0,
//                    .handle = (uint8_t *)fname.c_str(),
//                    .data = nullptr,
//                    .child_addrs = nullptr};

//     // for (uint8_t i = 0; i < indexes.size(); i++) {
//     // debug.printf("i = %u\r\n", i);
//     // debug.printf("%u\r\n", indexes[i - 1]);
//     // debug.printf("%u\r\n", indexes[i - 2]);

//     // }

//     // for (uint8_t i = 0; i < fname.length(); i++) {
//     //     debug.printf("%u\r\n", file.handle[i]);
//     // }
//     // nvm.write(system.free_space_address, );
// }

#include "bonsai/bonsai.hpp"

void Bonsai::put(file_t &file) {
    uint32_t fsize = file.size();
    uint8_t padding = ROW_SIZE - (fsize % ROW_SIZE);
    uint32_t total_size = fsize + padding;
    uint8_t buffer[total_size];
    memset(buffer, 0, total_size);
    file.to_buffer(buffer);

    uint32_t buffer_index = 0;
    for (uint8_t i = 0; i < total_size / ROW_SIZE; i++) {
        nvm.erase_row(fsa);
        for (uint8_t j = 0; j < 4; j++) {
            uint8_t page_buffer[PAGE_SIZE];
            memcpy(page_buffer, buffer + buffer_index, PAGE_SIZE);
            nvm.write_page(fsa - PAGE_SIZE, page_buffer);

            buffer_index += PAGE_SIZE;
            fsa = update_fsa(fsa, PAGE_SIZE);
        }
    }
}

void Bonsai::put(file_t &file, uint32_t address) {
    fsa = address;
    put(file);
}

void Bonsai::put_blank_file(const std::string name, uint32_t parent_address, uint32_t address) {
    if (!address) {
        address = fsa;
    }
    file_t file = {
        .handle_size = (uint8_t)name.size(),
        .data_size = 0,
        .parent_addr = parent_address,  // reverse_addr(parent_address), // All addresses should be stored LSB
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

    uint32_t parent_addr = *p++         |   //
                           *p++ << 8    |   //
                           *p++ << 16   |   //
                           *p++ << 24;      //

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
    fsa = address;
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

uint32_t Bonsai::find(const uint32_t root, const std::string handle) {
    const uint8_t size = 100;
    uint32_t stack[size];
    int32_t index = -1;

    stack[++index] = root;

    while (index >= 0) {
        uint32_t current_addr = stack[index--];

        const auto file = get(current_addr);

        char buffer[file.handle_size];
        memcpy(buffer, file.handle, file.handle_size);
        buffer[file.handle_size] = '\0';

        if (!strcmp(handle.c_str(), buffer)) {
            return current_addr;
        }
        if (file.num_child_addrs) {
            for (uint8_t i = 0; i < file.num_child_addrs; i++) {
                auto addr = file.child_addrs[i];
                stack[++index] = addr;
            }
        }
    }
    return U32_FLASH_RESET_VALUE;
}

uint32_t Bonsai::find(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    std::string handle;

    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);
        path.erase(0, pos + 1);
    }

    auto addr = find(ROOT_DIRECTORY_ADDRESS, handle);
    return addr;
}
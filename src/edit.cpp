#include "bonsai/bonsai.hpp"

void Bonsai::edit_file_handle(const uint32_t address, const std::string handle) {
    auto file = get(address);
    file.handle = (uint8_t *)handle.c_str();
    file.handle_size = (uint8_t)handle.size();
    put(file, address);
}

void Bonsai::edit_file_handle(std::string path, const std::string handle) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    std::string old_handle;

    while ((pos = path.find("/")) != std::string::npos) {
        old_handle = path.substr(0, pos);
        path.erase(0, pos + 1);
    }

    auto addr = find(ROOT_DIRECTORY_ADDRESS, old_handle);

    if (addr != U32_FLASH_RESET_VALUE) {
        edit_file_handle(addr, handle);
    }
}

void Bonsai::edit_file_data(const uint32_t address, const std::string data) {
    auto file = get(address);
    file.data = (uint8_t *)data.c_str();
    file.data_size = (uint8_t)data.size();
    put(file, address);
}

void Bonsai::edit_file_parent_addr(const uint32_t address, const uint32_t parent_addr) {
    auto file = get(address);
    file.parent_addr = parent_addr;
    put(file, address);
}

void Bonsai::add_child_addr(const uint32_t address, uint32_t child_addr) {
    auto file = get(address);

    // Return early if this file already has this child
    if (file.num_child_addrs) {
        for (uint8_t i = 0; i < file.num_child_addrs; i++) {
            uint8_t *p = (uint8_t *)(file.child_addrs + i);
            auto check = bytes_to_u32(p);
            if (check == child_addr) {
                return;
            }
            p += 4;
        }
    }

    file.num_child_addrs++;
    uint32_t buffer[file.num_child_addrs];
    memcpy(buffer, file.child_addrs, (file.num_child_addrs - 1) * sizeof(uint32_t));
    buffer[file.num_child_addrs - 1] = child_addr;
    file.child_addrs = buffer;
    put(file, address);
}

void Bonsai::remove_child_addr(const uint32_t address, const uint32_t child_addr) {
    auto file = get(address);
    uint32_t buffer[file.num_child_addrs];
    memcpy(buffer, file.child_addrs, file.num_child_addrs * sizeof(uint32_t));

    for (uint8_t i = 0; i < file.num_child_addrs; i++) {
        if (buffer[i] == child_addr) {
            for (uint8_t j = i; j < file.num_child_addrs - 1; j++) {
                buffer[j] = buffer[j + 1];
            }
            file.num_child_addrs--;
            file.child_addrs = buffer;
            put(file, address);
            return;
        }
    }
}
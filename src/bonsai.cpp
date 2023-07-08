#include "bonsai/bonsai.hpp"
#include "bonsai/file.hpp"
#include "bonsai/system_file.hpp"

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

    auto system_file_data = get_sfd();
    fsa = system_file_data->fsa();

    if (fsa == U32_FLASH_RESET_VALUE) {
        init();
    }
}

void Bonsai::init(void) {
    write_fsa(ROOT_DIRECTORY_ADDRESS + ROW_SIZE);
    put_blank_file("root", 0x00, ROOT_DIRECTORY_ADDRESS);
}

part_t Bonsai::get_part(void) {
    return part; //
}

void Bonsai::erase(void) {
    uint32_t address = SYSTEM_FILE_ADDRESS;
    while (address < BONSAI_MEMORY_END) {
        nvm.erase_row(address);
        address += ROW_SIZE;
    }
    write_fsa(ROOT_DIRECTORY_ADDRESS);
}

system_file_data_t *Bonsai::get_sfd(void) {
    auto file = get(SYSTEM_FILE_ADDRESS);
    return (system_file_data_t *)file.data;
}

uint32_t Bonsai::read_fsa(void) {
    auto file = get(SYSTEM_FILE_ADDRESS);
    auto system_file_data = (system_file_data_t *)file.data;
    return system_file_data->fsa();
}

void Bonsai::write_fsa(uint32_t address) {
    if (!address) {
        address = fsa;
    }
    std::string handle = "system_file";
    std::array<uint8_t, 4> fsa_bytes;
    fsa_bytes[0] = (uint8_t)((address >> 24) & 0xFF);
    fsa_bytes[1] = (uint8_t)((address >> 16) & 0xFF);
    fsa_bytes[2] = (uint8_t)((address >> 8) & 0xFF);
    fsa_bytes[3] = (uint8_t)((address)&0xFF);

    file_t file = {
        .handle_size = (uint8_t)handle.size(),
        .data_size = (uint16_t)fsa_bytes.size(),
        .parent_addr = 0x00,
        .num_child_addrs = 0,
        .handle = (uint8_t *)handle.c_str(),
        .data = (uint8_t *)fsa_bytes.data(),
        .child_addrs = nullptr,
    };
    memcpy(file.data, fsa_bytes.data(), 4);
    put(file, SYSTEM_FILE_ADDRESS);
}

uint32_t Bonsai::update_fsa(uint32_t address, uint32_t increment) {
    return address + increment; //
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

void Bonsai::edit_file_handle(const uint32_t address, const std::string handle) {
    auto file = get(address);
    file.handle = (uint8_t *)handle.c_str();
    file.handle_size = (uint8_t)handle.size();
    put(file, address);
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

void Bonsai::create_file(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    uint32_t current_parent_addr = ROOT_DIRECTORY_ADDRESS;
    uint32_t prev_parent_addr = current_parent_addr;
    std::string handle;

    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);

        const auto addr = find(current_parent_addr, handle);
        auto current = fsa;

        if (addr == U32_FLASH_RESET_VALUE) { // if handle is not a child of current parent addr then it doesn't exist
            put_blank_file(handle, current_parent_addr, current);
            write_fsa();
            edit_file_parent_addr(current, current_parent_addr);
        }

        add_child_addr(prev_parent_addr, current);
        prev_parent_addr = current_parent_addr;
        current_parent_addr = addr;

        // debug.printf("%s\r\n", handle.c_str());

        path.erase(0, pos + 1);
    }
    // debug.printf("%s\r\n", handle.c_str());

    put_blank_file(handle, fsa, current_parent_addr);
    write_fsa();
}

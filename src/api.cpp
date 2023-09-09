#include "bonsai/bonsai.hpp"

void Bonsai::create_file(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    uint32_t prev_addr = ROOT_DIRECTORY_ADDRESS;
    std::string handle;

    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);

        auto curr_addr = find(prev_addr, handle);

        if (curr_addr == U32_FLASH_RESET_VALUE) {
            curr_addr = fsa;
            put_blank_file(handle, prev_addr, curr_addr);
            auto saved_fsa = fsa;
            add_child_addr(prev_addr, curr_addr);
            fsa = saved_fsa;
        }

        prev_addr = curr_addr;
        path.erase(0, pos + 1);
    }
}

void Bonsai::delete_file(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    auto saved_fsa = fsa;

    auto addr = find(path);

    if (addr != U32_FLASH_RESET_VALUE) {
        auto file = get(addr);
        remove_child_addr(file.parent_addr, addr);
        delete_recursive(addr);
        fsa = saved_fsa;
    }
}

void Bonsai::delete_recursive(uint32_t root_addr) {
    uint32_t address_stack[32];
    size_t index = 0;

    address_stack[index++] = root_addr;

    while (index) {
        uint32_t addr = address_stack[--index];

        auto file = get(addr);

        if (file.num_child_addrs) {
            for (uint8_t i = 0; i < file.num_child_addrs; i++) {
                address_stack[index++] = file.child_addrs[i];
            }
        }
        del(addr);
    }
}

void Bonsai::move_file(std::string dest, std::string src) {}

void Bonsai::read_file(std::string path) {}

void Bonsai::write_to_file(std::string path, std::string data) {}
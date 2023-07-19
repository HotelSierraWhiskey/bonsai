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

    size_t pos = 0;
    std::string handle;
    auto saved_fsa = fsa;

    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);
        path.erase(0, pos + 1);
    }

    auto addr = find(ROOT_DIRECTORY_ADDRESS, handle);

    if (addr != U32_FLASH_RESET_VALUE) {
        auto file = get(addr);
        remove_child_addr(file.parent_addr, addr);
        del(addr);
        fsa = saved_fsa;
    }
}
#include "bonsai/bonsai.hpp"

void Bonsai::create_file(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    uint32_t current_addr = ROOT_DIRECTORY_ADDRESS;
    uint32_t prev_addr = current_addr;
    std::string handle;

    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);

        // debug.printf("TOP OF WHILE. (handling %s)\r\n", handle.c_str());

        const auto found_addr = find(current_addr, handle);
        // debug.printf("found addr: %X\r\n", found_addr);

        const auto saved = fsa;

        if (found_addr == U32_FLASH_RESET_VALUE) {
            // debug.printf("new file to add at fsa. adding...\r\n");
            put_blank_file(handle.c_str(), current_addr, saved);
            // debug.printf("writing fsa...\r\n");
            write_fsa();
            // debug.printf("adding child to prev file...\r\n");
            add_child_addr(prev_addr, saved);
        }

        if (found_addr != ROOT_DIRECTORY_ADDRESS) {
            // debug.printf("not root dir adding parent...\r\n");
            edit_file_parent_addr(saved, prev_addr);
        }

        prev_addr = current_addr;
        current_addr = found_addr;

        path.erase(0, pos + 1);
    }
}

void Bonsai::delete_file(std::string path) {
    if (path.back() != '/') {
        path += '/';
    }

    size_t pos = 0;
    uint32_t current_addr = ROOT_DIRECTORY_ADDRESS;
    uint32_t prev_addr = current_addr;
    std::string handle;

    uint32_t found_addr = U32_FLASH_RESET_VALUE;

    const auto saved = fsa;
    while ((pos = path.find("/")) != std::string::npos) {
        handle = path.substr(0, pos);
        found_addr = find(current_addr, handle);
        prev_addr = current_addr;
        current_addr = found_addr;
        path.erase(0, pos + 1);
    }
    found_addr = find(current_addr, handle);
    del(found_addr);
    remove_child_addr(prev_addr, found_addr);
}
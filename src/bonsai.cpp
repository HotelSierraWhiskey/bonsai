#include "bonsai/bonsai.hpp"

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
    put_blank_file("root", ROOT_DIRECTORY_ADDRESS, ROOT_DIRECTORY_ADDRESS);
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
    fsa_bytes[0] = (uint8_t)(address & 0xFF);
    fsa_bytes[1] = (uint8_t)((address >> 8) & 0xFF);
    fsa_bytes[2] = (uint8_t)((address >> 16) & 0xFF);
    fsa_bytes[3] = (uint8_t)((address >> 24) & 0xFF);

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


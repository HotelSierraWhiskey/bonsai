#include "bonsai/nvmctrl.hpp"

Nvmc::Nvmc(void) {
    nvmctrl_AHB_clock_enable();
    nvmctrl_APBB_clock_enable();
    nvmctrl = get_nvmctrl();
    nvm_memory = (volatile uint16_t *)0x00U;
}

void Nvmc::execute(CMD cmd) {
    const uint8_t cmdex_key = 0xA5U;
    nvmctrl->NVMCTRL_CTRLA = (cmdex_key << 8) | (uint8_t)cmd;
    while (!(nvmctrl->NVMCTRL_INTFLAG & 1)) {
        continue;
    }
}

void Nvmc::write(uint32_t address, uint8_t *buffer, uint8_t length) {
    execute(CMD::PBC);
    uint32_t nvm_address = address >> 1;

    for (uint16_t i = 0; i < length; i += 2) {
        uint16_t data = buffer[i];
        if (i < (length - 1)) {
            data |= (buffer[i + 1] << 8);
        }
        nvm_memory[nvm_address++] = data;
    }
    nvmctrl->NVMCTRL_ADDR = nvm_address;
    execute(CMD::WP);
}

void Nvmc::erase_row(uint32_t address) {
    nvmctrl->NVMCTRL_ADDR = address >> 1;
    execute(CMD::ER);
}

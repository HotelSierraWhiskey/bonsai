#ifndef BONSAI_NVMCTRL_HPP
#define BONSAI_NVMCTRL_HPP

#include "includes.hpp"

/* MCLOCK */
#define MCLOCK_BASE_ADDRESS 0x40000800
#define AHBMASK_OFFSET 0x10
#define APBBMASK_OFFSET 0x18
#define MCLOCK_AHBMASK_ADDRESS (MCLOCK_BASE_ADDRESS + AHBMASK_OFFSET)
#define MCLOCK_APBBMASK_ADDRESS (MCLOCK_BASE_ADDRESS + APBBMASK_OFFSET)

constexpr void nvmctrl_AHB_clock_enable(void) {
    uint32_t *ahb_mask_register = (uint32_t *)MCLOCK_AHBMASK_ADDRESS;
    *ahb_mask_register |= (1 << 0x05);
}

constexpr void nvmctrl_APBB_clock_enable(void) {
    uint32_t *apbb_mask_register = (uint32_t *)MCLOCK_APBBMASK_ADDRESS;
    *apbb_mask_register |= (1 << 0x02);
}

/* ************************************************************************** */

/* NVMCTRL */
#ifndef NVMCTRL_BASE_ADDRESS
    #define NVMCTRL_BASE_ADDRESS 0x41004000
#endif

typedef struct {
    volatile uint16_t NVMCTRL_CTRLA;
    volatile uint8_t Reserved1[0x02];
    volatile uint32_t NVMCTRL_CTRLB;
    volatile uint32_t NVMCTRL_PARAM;
    volatile uint8_t NVMCTRL_INTENCLR;
    volatile uint8_t Reserved2[0x03];
    volatile uint8_t NVMCTRL_INTENSET;
    volatile uint8_t Reserved3[0x03];
    volatile uint8_t NVMCTRL_INTFLAG;
    volatile uint8_t Reserved4[0x03];
    volatile uint16_t NVMCTRL_STATUS;
    volatile uint8_t Reserved5[0x02];
    volatile uint32_t NVMCTRL_ADDR;
    volatile uint16_t NVMCTRL_LOCK;
    volatile uint8_t Reserved6[0x06];
    volatile uint32_t NVMCTRL_PBLDATA0;
    volatile uint32_t NVMCTRL_PBLDATA1;
} nvmctrl_regs_t;

static constexpr nvmctrl_regs_t *get_nvmctrl(void) {
    auto nvmctrl = (nvmctrl_regs_t *)NVMCTRL_BASE_ADDRESS;
    return nvmctrl;
}

/* ************************************************************************** */

class Nvmc {
    enum class CMD {
        ER = 0x02U,
        WP = 0x04U,
        PBC = 0x44U,
    };

  private:
    void execute(CMD cmd);
    volatile uint16_t *nvm_memory;

  public:
    nvmctrl_regs_t *nvmctrl;
    Nvmc(void);
    void write_page(uint32_t address, uint8_t *buffer);
    void erase_row(uint32_t address);
};

#endif
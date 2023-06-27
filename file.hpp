#ifndef BONSAI_FILE_HPP
#define BONSAI_FILE_HPP

#include "includes.hpp"

typedef struct {
    uint8_t handle_size;
    uint16_t data_size;
    uint32_t parent_addr;
    uint8_t num_child_addrs;
    uint8_t *handle;
    uint8_t *data;
    uint32_t *child_addrs;
    uint32_t size(void) {
        return handle_size + sizeof(handle_size) +   //
               data_size + sizeof(data_size) +       //
               sizeof(parent_addr) +                 //
               sizeof(num_child_addrs) +             //
               (num_child_addrs * sizeof(uint32_t)); //
    }
    uint32_t location(void) {
        auto loc = (uint32_t)(handle - 8);
        return loc;
    }
    void to_buffer(uint8_t *buffer) {
        uint8_t mask = 0xFF;
        uint32_t index = 0;

        buffer[index++] = handle_size;

        buffer[index++] = (data_size >> 8) & mask;
        buffer[index++] = data_size & mask;

        buffer[index++] = (parent_addr >> 24) & mask;
        buffer[index++] = (parent_addr >> 16) & mask;
        buffer[index++] = (parent_addr >> 8) & mask;
        buffer[index++] = parent_addr & mask;

        buffer[index++] = num_child_addrs;

        if (handle_size) {
            for (uint16_t i = 0; i < handle_size; i++) {
                buffer[index++] = handle[i];
            }
        }
        if (data_size) {
            for (uint16_t i = 0; i < data_size; i++) {
                buffer[index++] = data[i];
            }
        }
        if (num_child_addrs) {
            for (uint8_t i = 0; i < num_child_addrs; i++) {
                uint8_t *p = (uint8_t *)(child_addrs + i);
                buffer[index++] = p[0];
                buffer[index++] = p[1];
                buffer[index++] = p[2];
                buffer[index++] = p[3];
            }
        }
        return;
    }
} file_t;

#endif

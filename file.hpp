#ifndef BONSAI_FILE_HPP
#define BONSAI_FILE_HPP

#include <cstdint>

#define MAX_HANDLE_SIZE 0xFE

struct file_t {
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
        // FIXME: if handle is nullptr, can it cause problems?
        return (uint32_t)(handle - 8); //
    }
    void to_buffer(uint8_t *buffer) {
        // FIXME: buffer accessed without checking if its nullptr, can cause SEGSIGV
        // TODO: add a test by passing nullptr to this function

        uint8_t mask = 0xFF;
        uint32_t index = 0;

        buffer[index++] = handle_size;

        buffer[index++] = data_size & mask;
        buffer[index++] = (data_size >> 8) & mask;

        buffer[index++] = parent_addr & mask;
        buffer[index++] = (parent_addr >> 8) & mask;
        buffer[index++] = (parent_addr >> 16) & mask;
        buffer[index++] = (parent_addr >> 24) & mask;

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
};

#endif

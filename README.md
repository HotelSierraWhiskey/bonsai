# Bonsai
### A tiny, persistent file manager for ARM-based embedded systems.


## Features
- Persistent directory structure with nested file creation.
- Supports all ATSAMC2x/ ATSAMD2x microcontrollers.
- Minimal configuration - automatic device identification.
- Built-in nonvolatile memory peripheral driver.

## File Structure
Once written, a file resides contiguously at a row boundary in flash memory.

A file may span multiple rows provided that it adheres to the size limits of its constituent fields, and that it fits within the Bonsai flash partition.

Its first four fields are fixed in size and make up the file header.
The size of succeeding fields - and therefore the file in its entirety - is derived from the header.
The last three fields vary in size and make up the body of the file structure.
Notably, a file does not store its own address, as the address of a file is the address of its first member.

<table class="tg">
<thead>
  <tr>
    <th class="tg-0pky">Handle Size</th>
    <th class="tg-0pky">Data Size</th>
    <th class="tg-0pky">Parent Address</th>
    <th class="tg-0pky"># of Child Addresses</th>
    <th class="tg-0pky">Handle</th>
    <th class="tg-0pky">Data</th>
    <th class="tg-0pky">Child Addresses</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td class="tg-0pky">1 byte</td>
    <td class="tg-0pky">2 bytes</td>
    <td class="tg-0pky">4 bytes</td>
    <td class="tg-0pky">1 byte</td>
    <td class="tg-0pky">variable</td>
    <td class="tg-0pky">variable</td>
    <td class="tg-0pky">variable</td>
  </tr>
  <tr>
    <td class="tg-c3ow" colspan="4">Header</td>
    <td class="tg-c3ow" colspan="3">Body</td>
  </tr>
</tbody>
</table>

Accordingly, the above table is expressed in firmware as a `file_t` structure.


```c++
typedef struct {
    uint8_t handle_size;        // the size of handle in bytes
    uint16_t data_size;         // the size of data in bytes
    uint32_t parent_addr;       // the address of this file's parent file
    uint8_t num_child_addrs;    // the number of children immediately descendant from this file
    uint8_t *handle;            // the handle or name of this file
    uint8_t *data;              // the actual data this file contains
    uint32_t *child_addrs;      // children addresses
} file_t;
```

The size of any file can be expressed in terms of a `file_t`, thus:
```c++
handle_size + sizeof(handle_size) +   // handle_size bytes plus one more for storing the size of handle_size
data_size + sizeof(data_size) +       // same for data
sizeof(parent_addr) +                 // four bytes for the parent address
sizeof(num_child_addrs) +             // one byte for storing the number of child addresses
(num_child_addrs * sizeof(uint32_t)); // four bytes for every child address
```

## Low Level API
The `Bonsai` class implements a set of methods for basic file reading and writing. 
These methods are not involved with file path traversal. Rather, they interact directly with addresses and are generally called by the top-level API.
They are:
```c++
void put(file_t &file);                             // writes a file to flash memory
file_t get(const uint32_t address);                 // retrieves a file from memory
void del(const uint32_t address);                   // erases all rows containing a file
void mov(const uint32_t dest, const uint32_t src);  // moves a file from src to dest
```
These methods are adequate for applications requiring basic recordkeeping. For applications involving any actual directory traversal, the top-level API should be used.
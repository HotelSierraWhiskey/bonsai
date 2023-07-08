# Bonsai
### A tiny, persistent file manager for ARM-based embedded systems.

![license](https://img.shields.io/badge/license-MIT-green)
![license](https://img.shields.io/badge/version-alpha-yellow)


<p align="center">
  <img src="docs/images/logo.png" />
</p>


## Features
- Persistent directory structure with nested file creation.
- Supports all ATSAMC2x/ ATSAMD2x microcontrollers.
- Minimal configuration - automatic device identification.
- Built-in nonvolatile memory peripheral driver.

## Example

```c++
Bonsai bonsai;
bonsai.create_file("root/config/networking");
```

## Contribute
Check out [how to contribute](docs/CONTRIBUTING.md).

## File Structure
Once written, a file resides contiguously at a row boundary in flash memory. A file may span multiple rows provided that it adheres to the size limits of its constituent fields, and that it fits within the Bonsai flash partition. Its first four fields are fixed in size and make up the file header.
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
    /* ... */
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

## The Root File, the System File, and the Free Space Address (FSA)
Upon initialization, Bonsai will look for a System File at the start of its flash partition. If one does not exist, Bonsai will create one as well as a Root File. The Root File is, unsurprisingly, the parent of all user-created files. Its handle is `root` and it is initialized without any data. The System File is a special file that resides one row before the Root File address. It has neither parent nor children addresses and is used for general memory management. Crucially, the System File stores the Free Space Address (FSA) which itself is used to determine the location of the next file to be written. This mechanism works such that when a file is written the FSA is incremented to the row address immediately following the end of the written file. Conversely, when a file is deleted the FSA is decremented to the row address at which the deleted file was located. It is therefore the case that the maximum size of a memory fragment is `ROW_SIZE - 1` or 255 bytes.


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


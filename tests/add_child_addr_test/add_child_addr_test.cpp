#include "add_child_addr_test.hpp"

/*

Checks for:
- correct child additions
- no child addr duplications
- num child addrs matches number of entries

*/

extern void add_child_addr_test(void) {
    Bonsai bonsai;
    bonsai.put_blank_file("test", ROOT_DIRECTORY_ADDRESS, 0x1b200);
    bonsai.add_child_addr(0x1b200, 0x1b300);
    bonsai.add_child_addr(0x1b200, 0x1b300); // intentional duplications...
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
    bonsai.add_child_addr(0x1b200, 0x1b500);
}



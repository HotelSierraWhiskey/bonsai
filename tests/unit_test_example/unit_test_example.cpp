/* This is an example of a unit test implementation file */

#include "unit_test_example.hpp"

void unit_test_example(void) {
    Bonsai bonsai;
    bonsai.put_blank_file("test_file", 0x00, 0x1b400);     // create a file
    bonsai.edit_file_data(0x1b400, "parent_data");         // write data to file
    bonsai.add_child_addr(0x1b400, 0x1b500);               // give the file a child address
    bonsai.put_blank_file("child_file", 0x1b400, 0x1b500); // create the child file
    bonsai.edit_file_data(0x1b500, "child data");          // write data to child file
}
#include "create_file_test.hpp"

/*

Checks for:
- correct child/parent linkage
- creation of files within an existing path

*/

void create_file_test(void) {
    Bonsai bonsai;
    bonsai.create_file("root/file1");
    bonsai.create_file("root/file1/file1_child2");
    bonsai.create_file("root/file1/file1_child3");
}
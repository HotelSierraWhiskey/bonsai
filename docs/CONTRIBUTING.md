## Contributing

### Preamble
I'm going to eventually integrate an easy way for contributers to compile and upload Bonsai firmware for themselves. For the time being, you can fork the repo, make your changes in a new branch, and run `make compile` just to be sure your changes are okay. Then submit a PR. I compile the firmware with arm-none-eabi-g++ but it will also compile with g++ just fine.

### Ways to Contribute
- Unit tests are always welcome. To write a unit test, create a directory with the name of your test in bonsai/tests. Write your test there. Check out the [unit test example](../tests/unit_test_example) and use it as a template.
- Refactors that better organize the project and/ or decrease LoC or are welcome. No dynamic memory allocation please.
- The firmware currently lacks a lot of documentation. If you're good at writing documentation, have at 'er.

### More on Unit Tests
Initialize a unit test template by running

`make test name_of_my_test`

`name_of_my_test` should be replaced with the name of your unit test. This command will generate a header and implementation file in the new directory bonsai/tests/name_of_my_test.

You can safely assume that all unit tests will be run on hardware without a preexisting Bonsai partition. Please try and keep any addressing between `0x19000` and `20000`. It will always be the case that some features are more ready for testing than others. Here is the example unit test provided in the tests directory.

```c++
/* Current directory: bonsai/tests/unit_test_example/ */
#include "unit_test_example.hpp"

void unit_test_example(void) {
    Bonsai bonsai;
    bonsai.put_blank_file("test_file", 0x00, 0x1b400);     // create a file
    bonsai.edit_file_data(0x1b400, "parent_data");         // write data to file
    bonsai.add_child_addr(0x1b400, 0x1b500);               // give the file a child address
    bonsai.put_blank_file("child_file", 0x1b400, 0x1b500); // create the child file
    bonsai.edit_file_data(0x1b500, "child data");          // write data to child file
}
```

Feel free to write larger tests than this one, but do try to test a specific feature. Once you submit a unit test, I'll run it on hardware and mark it as either pass/ fail.
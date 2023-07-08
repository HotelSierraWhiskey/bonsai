## Contributing

### Preamble
I'm going to eventually integrate an easy way for contributers to compile and upload Bonsai firmware for themselves. For the time being, you can clone the repo, make your changes, and run `make compile` just to be sure your changes are okay. I compile the firmware with arm-none-eabi-g++ but it will also compile with g++ just fine.

### Ways to Contribute
- Unit tests are always welcome. To write a unit test, create a directory with the name of your test in bonsai/tests. Write your test there. Check out the unit test example there already and use it as a template.
- Refactors that better organize the project and/ or decrease LoC or are welcome. No dynamic memory allocation please.
- The firmware currently lacks a lot of documentation. If you're good at writing documentation, have at 'er.

### More on Unit Tests
You can safely assume that all unit tests will be run on hardware without a preexisting Bonsai partition. Please try and keep any addressing between `0x19000` and `20000`. It will always be the case that some features are more ready for testing than others. As of this writing, the high level API is still being written. So it's probably a good idea to limit any units tests to the basic methods like `put`, `get`, `mov`, `del`, `write_fsa` and the file editing methods. Here is the example unit test provided in the tests directory.

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

Feel free to write larger tests than this one, but do try to test a specific feature or interation. Once you submit a unit test, I'll run it on hardware and mark it as either pass/ fail.
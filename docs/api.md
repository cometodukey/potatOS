# API

## Internal

### Architecture Specific

#### Functions

* `init_arch`          - Initialse architecture specific features
* `console_put_char`   - Write an ASCII character to the console
* `console_put_string` - Write n bytes to the console
* `serial_put_char`    - Write an ASCII character to serial
* `serial_put_string`  - Write n bytes to serial
* `pmm_alloc`          - Allocate a physical page
* `pmm_zalloc`         - Allocate, and zero out, a physical page
* `pmm_free`           - Free a physical page
* `TODO - paging API`  - TODO
* `do_stack_trace`     - Print a stack trace
* `dump_registers`     - Dump the register contents
* `idle`               - Sleep until an interrupt occurs
* `hang`               - Stop the current CPU

## Userland

TODO

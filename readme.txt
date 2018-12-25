The project consist of these files:
1. stm32_boot_client.cpp/hpp - the core of factory bootloader client. There are almost all commands that stm32 boot can accept.
2. stm32_io_pc.cpp/hpp - platform dependend interface to communicate with serial port, make system delay and configASSERT. Rewrite it under your platform.
3. stm32bootpc.cpp/hpp - just an example of using the core for ibm pc. It must be your platform-dependend software.

These software are compiled with GCC 7.3.0 with a whole command string:
g++ -Wall -o stm32bootpc.exe -pedantic -pedantic-errors -ansi -std=c++11
-Werror -Wextra -Wconversion -Winit-self -Wunreachable-code
-Wstrict-overflow=5 -Wshadow -Wcast-qual -Wcast-align  stm32_boot_client.cpp stm32_io_pc.cpp stm32bootpc.cpp

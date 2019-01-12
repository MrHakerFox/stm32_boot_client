CPPC=g++

CPPFLAGS=-c -Wall -pedantic -pedantic-errors -ansi -std=c++11 -Werror -Wextra -Wconversion
CPPFLAGS+=-Winit-self -Wunreachable-code -Wstrict-overflow=5 -Wshadow -Wcast-qual -Wcast-align 


all: target

target: stm32_boot_client.o stm32_io_pc.o stm32bootpc.o
	$(CPPC) stm32_boot_client.o stm32_io_pc.o stm32bootpc.o -o stm32bootpc.exe

stm32_boot_client.o: stm32_boot_client.cpp
	$(CPPC) $(CPPFLAGS) stm32_boot_client.cpp

stm32_io_pc.o: stm32_io_pc.cpp
	$(CPPC) $(CPPFLAGS) stm32_io_pc.cpp

stm32bootpc.o: stm32bootpc.cpp
	$(CPPC) $(CPPFLAGS) stm32bootpc.cpp

clean:
	rm *.o stm32bootpc.exe

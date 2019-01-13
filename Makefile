TARGET=stm32bootpc.exe

CXX=g++
LD=g++

CXXFLAGS=-c -Wall -pedantic -pedantic-errors -ansi -std=c++11 -Werror -Wextra -Wconversion
CXXFLAGS+=-Winit-self -Wunreachable-code -Wstrict-overflow=5 -Wshadow -Wcast-qual -Wcast-align 

LDFLAGS=


all: $(TARGET)

$(TARGET): stm32_boot_client.o stm32_io_pc.o stm32bootpc.o
	$(LD) -o $@ $^ $(LDFLAGS)

clean:
	rm *.o $(TARGET)

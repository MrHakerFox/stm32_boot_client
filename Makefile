TARGET=stm32bootpc.exe

CXX=g++

CXXFLAGS=-c -Wall -pedantic -pedantic-errors -ansi -std=c++11 -Werror -Wextra -Wconversion
CXXFLAGS+=-Winit-self -Wunreachable-code -Wstrict-overflow=5 -Wshadow -Wcast-qual -Wcast-align 


all: $(TARGET)

$(TARGET): stm32_boot_client.o stm32_io_pc.o stm32bootpc.o
	$(CXX) stm32_boot_client.o stm32_io_pc.o stm32bootpc.o -o $(TARGET)

stm32_boot_client.o: stm32_boot_client.cpp
	$(CXX) $(CXXFLAGS) stm32_boot_client.cpp

stm32_io_pc.o: stm32_io_pc.cpp
	$(CXX) $(CXXFLAGS) stm32_io_pc.cpp

stm32bootpc.o: stm32bootpc.cpp
	$(CXX) $(CXXFLAGS) stm32bootpc.cpp

clean:
	rm *.o $(TARGET)

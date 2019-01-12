all: target

target: stm32_boot_client.o stm32_io_pc.o stm32bootpc.o
	g++ stm32_boot_client.o stm32_io_pc.o stm32bootpc.o -o stm32bootpc.exe

stm32_boot_client.o: stm32_boot_client.cpp
	g++ -c stm32_boot_client.cpp

stm32_io_pc.o: stm32_io_pc.cpp
	g++ -c stm32_io_pc.cpp

stm32bootpc.o: stm32bootpc.cpp
	g++ -c stm32bootpc.cpp

clean:
	rm *.o stm32bootpc.exe

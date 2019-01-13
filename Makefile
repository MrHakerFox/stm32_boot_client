TARGET=stm32bootpc.exe
CXXSRC=$(wildcard *.cpp)
OBJ=$(CXXSRC:.cpp=.o)
DEPS=$(OBJ:.o=.d)

CXX=g++
LD=g++

CXXFLAGS=-c -Wall -pedantic -pedantic-errors -ansi -std=c++11 -Werror -Wextra -Wconversion
CXXFLAGS+=-Winit-self -Wunreachable-code -Wstrict-overflow=5 -Wshadow -Wcast-qual -Wcast-align 

LDFLAGS=

all: $(TARGET)

$(TARGET): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm *.o $(TARGET)

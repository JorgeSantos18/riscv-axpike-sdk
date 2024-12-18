RISCV_PREFIX=riscv64-unknown-elf-

CC			=$(RISCV_PREFIX)gcc
LD			=$(RISCV_PREFIX)gcc

CFLAGS	= -O0 -g -std=c++11
LFLAGS	= -lm -lnuma 
HEADERS = src
INCLUDE =

TARGET	=mm
EXE			=bin/$(TARGET).rv

SRC_FILES = mm.cpp
OBJ_FILES = $(addprefix obj/,$(notdir $(SRC_FILES:.cpp=.o)))

.PHONY: all clean
	
all: DIR $(EXE)

DIR: clean
	@mkdir bin -p
	@mkdir obj -p

$(EXE): $(OBJ_FILES)
	$(LD) $^ $(LFLAGS) -o $@

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@rm -rf bin
	@rm -rf obj

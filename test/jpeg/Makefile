RISCV_PREFIX ?= riscv64-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc
RISCV_GXX ?= $(RISCV_PREFIX)g++

CC			:= $(RISCV_GCC)
LD			:= $(RISCV_GCC)



TARGET=jpeg

SRC_FILES := src/dct.cpp src/encoder.cpp src/huffman.cpp src/marker.cpp src/quant.cpp src/rgbimage.cpp src/jpeg.cpp
OBJ_FILES := $(addprefix obj/,$(notdir $(SRC_FILES:.cpp=.o))) 

#AFLAGS = -DAPPROX
CFLAGS		:= -O0 -Wall -std=c++11  -g $(AFLAGS)
LFLAGS = -lm -lnuma $(AFLAGS)

TMP=/tmp/

EXE = bin/$(TARGET).rv



all: DIR $(EXE)

DIR:
	mkdir -p obj 
	mkdir -p bin

$(EXE): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^  $(LFLAGS) 

get_phy_addr: src/get_phy_addr.cpp
	$(RISCV_GXX) $(CFLAGS) -o $@ $^  $(LFLAGS) 	


obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf bin
	rm -rf obj

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>


#define OUT_BUFFER_SIZE 500000 // in bytes

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PFN_MASK ((1ULL << 55) - 1)

int64_t get_physical_address(int fd, uint64_t virtual_addr) {
    uint64_t virtual_page = (uint64_t)virtual_addr / PAGE_SIZE;
    uint64_t offset = virtual_page * sizeof(uint64_t);
    uint64_t page_frame_number = 0;

    // int fd = open("/proc/self/pagemap", O_RDONLY);
    // if (fd < 0) {
    //     perror("Failed to open /proc/self/pagemap");
    //     exit(EXIT_FAILURE);
    // }

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("Failed to seek in /proc/self/pagemap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (read(fd, &page_frame_number, sizeof(uint64_t)) != sizeof(uint64_t)) {
        perror("Failed to read /proc/self/pagemap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // close(fd);

    // Check if the page is present
    if (!(page_frame_number & (1ULL << 63))) {
        fprintf(stderr, "Page not present in memory\n");
        return 0;
    }

    // Extract the page frame number and calculate the physical address
    page_frame_number &= PFN_MASK;
    uint64_t physical_address = (page_frame_number * PAGE_SIZE) + ((uint64_t)virtual_addr % PAGE_SIZE);

    return physical_address;
}


int main (int argc, const char* argv[]) {
	uint32_t pid;
    uint64_t virtual_addr_base;
    uint64_t virtual_addr_end;

    uint64_t virtual_page;
    uint64_t offset;

    uint64_t page_frame_number = 0;

    pid = std::stoul(argv[1]);
    virtual_addr_base = std::stoull(argv[2], nullptr, 16);
    virtual_addr_end = std::stoull(argv[3], nullptr, 16);

    std::string pagemap = "/proc/" + std::to_string(pid) + "/pagemap";
    int fd = open(pagemap.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /proc/self/pagemap");
        exit(EXIT_FAILURE);
    }

    while(virtual_addr_base < virtual_addr_end){

        uint64_t physical_address = get_physical_address(fd, virtual_addr_base);

        std::cout << "virt: 0x" << std::hex << virtual_addr_base << " phy: " << "0x" << std::hex << physical_address << std::endl;

        virtual_addr_base+=PAGE_SIZE;
    }

    close(fd);

    
	return 0;
}


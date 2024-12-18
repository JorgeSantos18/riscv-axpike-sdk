/*
 * rgbimage.c
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */

#include "rgbimage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <numa.h>
#include <numaif.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>

#define NUMA_NODE 1


void initRgbImage(RgbImage* image) {
	image->w = 0;
	image->h = 0;
	image->pixels = NULL;
	image->meta = NULL;
}

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PFN_MASK ((1ULL << 55) - 1)

uint64_t get_physical_address(void *virtual_addr) {
    // uint64_t virtual_page = (uint64_t)virtual_addr / PAGE_SIZE;
    // uint64_t offset = virtual_page * sizeof(uint64_t);
    // uint64_t page_frame_number = 0;

    // int fd = open("/proc/self/pagemap", O_RDONLY);
    // if (fd < 0) {
    //     perror("Failed to open /proc/self/pagemap");
    //     exit(EXIT_FAILURE);
    // }

    // if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
    //     perror("Failed to seek in /proc/self/pagemap");
    //     close(fd);
    //     exit(EXIT_FAILURE);
    // }

    // if (read(fd, &page_frame_number, sizeof(uint64_t)) != sizeof(uint64_t)) {
    //     perror("Failed to read /proc/self/pagemap");
    //     close(fd);
    //     exit(EXIT_FAILURE);
    // }

    // close(fd);

    // // Check if the page is present
    // if (!(page_frame_number & (1ULL << 63))) {
    //     fprintf(stderr, "Page not present in memory\n");
    //     exit(EXIT_FAILURE);
    // }

    // // Extract the page frame number and calculate the physical address
    // page_frame_number &= PFN_MASK;
    // uint64_t physical_address = (page_frame_number * PAGE_SIZE) + ((uint64_t)virtual_addr % PAGE_SIZE);

	uint64_t physical_address = 0;
    return physical_address;
}

int readCell(FILE *fp, char* w) {
	int c;
	int i;

	w[0] = 0;
	for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) {
		if (c == ' ' || c == '\t') {
			if (w[0] != '\"')
				continue;
		}

		if (c == ',' || c == '\n') {
			if (w[0] != '\"')
				break;
			else if (c == '\"') {
				w[i] = c;
				i++;
				break;
			}
		}

		w[i] = c;
		i++;
	}
	w[i] = 0;

	return c;
}

//int loadRgbImage(const char* fileName, RgbImage* image) {
int loadRgbImage(RgbImage* image) {
	int c;
	int i;
	int j;
	char w[256];
	RgbPixel** pixels;
	FILE *fp;

	//printf("Loading %s ...\n", fileName);

	//fp = fopen(fileName, "r");
	//if (!fp) {
	//	printf("Warning: Oops! Cannot open %s!\n", fileName);
	//	return 0;
  //}
  fp = stdin;

	c = readCell(fp, w);
	image->w = atoi(w);
	c = readCell(fp, w);
	image->h = atoi(w);

	//printf("%d x %d\n", image->w, image->h);

	// printf("Aqui h1 \n");
	pixels = (RgbPixel**)malloc(image->h * sizeof(RgbPixel*));
	// pixels = (RgbPixel**)numa_alloc_onnode(image->h * sizeof(RgbPixel*), NUMA_NODE);

// printf("Aqui h2 \n");

	if (pixels == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

		//fclose(fp);

		return 0;
	}




	c = 0;
	for(i = 0; i < image->h; i++) {
		// pixels[i] = (RgbPixel*)malloc(image->w * sizeof(RgbPixel));
		pixels[i] = (RgbPixel*)numa_alloc_onnode(image->w * sizeof(RgbPixel), NUMA_NODE);
		if (pixels[i] == NULL) {
			c = 1;
			break;
		}

	}

// printf("Aquids h3 \n");
	// printf("Base address: %p\n", pixels);
	// printf("Physical address: 0x%lx\n", get_physical_address(pixels));

	if (c == 1) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

		for (i--; i >= 0; i--)
			numa_free(pixels[i], image->w * sizeof(RgbPixel));
		free(pixels);
		// numa_free(pixels, image->h * sizeof(RgbPixel*));

		//fclose(fp);

		return 0;
	}

	// printf("Aqui h3.1 \n");
	// printf("Aqui h3.x h %d\n", image->h);
	// printf("Aqui h3.x w %d\n", image->w);
	// printf("Physical address: 0x%lx\n", get_physical_address(w));


	for(i = 0; i < image->h; i++) {
		
		for(j = 0; j < image->w; j++) {
			
			c = readCell(fp, w);
			pixels[i][j].r = atoi(w);

			c = readCell(fp, w);
			pixels[i][j].g = atoi(w);

			c = readCell(fp, w);
			pixels[i][j].b = atoi(w);

			// printf("Aqui h3.4 : %d 0x%lx\n", j, get_physical_address(&pixels[i][j]));
		}
		// printf("Aqui h3.3 : %d 0x%lx\n", i,  get_physical_address(pixels[i]));
		// printf("Aqui h3.2 \n");
		// printf("Base address: %p\n", pixels[i]);
		// printf("Physical address: 0x%lx\n", get_physical_address( pixels[i]));
	}

	raise(SIGTSTP);
	// printf("Aqui h3.3 \n");
	image->pixels = pixels;


// printf("Aqui h4 \n");
	c = readCell(fp, w);
	// image->meta = (char*)malloc(strlen(w) * sizeof(char));
	image->meta = (char*)numa_alloc_onnode(strlen(w) * sizeof(char), NUMA_NODE);
	if(image->meta == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

		for (i = 0; i < image->h; i++)
			numa_free(pixels[i], image->w * sizeof(RgbPixel));
		//numa_free(pixels, image->h * sizeof(RgbPixel*));
		free(pixels);

		//fclose(fp);

		return 0;

	}
	strcpy(image->meta, w);

	raise(SIGTSTP);

// printf("Aqui h 5\n");
	//printf("%s\n", image->meta);

	image->w = (image->w / 8) * 8;
	image->h = (image->h / 8) * 8;
	//printf("w=%d x h=%d\n", image->w, image->h);

	return 1;
}

int saveRgbImage(RgbImage* image, const char* fileName, float scale) {
	int i;
	int j;
	FILE *fp;

	//printf("Saving %s ...\n", fileName);

	fp = fopen(fileName, "w");
	if (!fp) {
		printf("Warning: Oops! Cannot open %s!\n", fileName);
		return 0;
	}

	fprintf(fp, "%d,%d\n", image->w, image->h);
	//printf("%d,%d\n", image->w, image->h);

	for(i = 0; i < image->h; i++) {
		for(j = 0; j < (image->w - 1); j++) {
			fprintf(fp, "%d,%d,%d,", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
		}
		fprintf(fp, "%d,%d,%d\n", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
	}

	fprintf(fp, "%s", image->meta);
	//printf("%s\n", image->meta);

	fclose(fp);

	return 1;
}

void freeRgbImage(RgbImage* image) {
	// printf("Hello c1 \n");
	int i;

	if (image->meta != NULL)
		numa_free(image->meta, 256* sizeof(char));
		// free(image->meta);

	if (image->pixels == NULL)
		return;

	for (i = 0; i < image->h; i++)
		if (image->pixels != NULL)
			numa_free(image->pixels[i], image->w * sizeof(RgbPixel));
			// free(image->pixels[i]);

	// numa_free(image->pixels, image->h * sizeof(RgbPixel*));
	free(image->pixels);
}

void makeGrayscale(RgbImage* image) {
	int i;
	int j;
	float luminance;

	float rC = 0.30;
	float gC = 0.59;
	float bC = 0.11;

	for(i = 0; i < image->h; i++) {
		for(j = 0; j < image->w; j++) {
			luminance =
				rC * image->pixels[i][j].r +
				gC * image->pixels[i][j].g +
				bC * image->pixels[i][j].b;

			image->pixels[i][j].r = (INT16)luminance;
			image->pixels[i][j].g = (INT16)luminance;
			image->pixels[i][j].b = (INT16)luminance;
		}
	}
}

void readMcuFromRgbImage(RgbImage* image, int x, int y, INT16* data) {
	int i, j;


	for (i = 0; i < 8; ++i) {
		for(j = 0; j < 8; ++j)
			data[i * 8 + j] = (image->pixels[y + i][x + j].r);
	}

#if 0
	for (i = 0; (i < 8) && ((y + i) < image->h); ++i) {
		for(j = 0; (j < 8) && ((x + i) < image->w); ++j)
			data[i * 8 + j] = (image->pixels[y + i][x + j].r);

		for(; j < 8; ++j)
			data[i * 8 + j] = (image->pixels[y + i][image->w - 1].r);
	}

	for (; i < 8; ++i) {
		for(j = 0; (j < 8) && ((x + i) < image->w); ++j)
			data[i * 8 + j] = (image->pixels[image->h - 1][x + j].r);

		for(; j < 8; ++j)
			data[i * 8 + j] = (image->pixels[image->h - 1][image->w - 1].r);
	}
#endif
}


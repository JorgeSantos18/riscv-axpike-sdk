/*
 * jpeg.c
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatype.h"
#include "jpegconfig.h"
#include "prototype.h"

#include "rgbimage.h"

#define OUT_BUFFER_SIZE 500000 // in bytes


int main (int argc, const char* argv[]) {
	//char inputFileName[64], outputFileName[64];
	UINT32 qualityFactor;
	UINT32 imageFormat;
  UINT32 res;

	UINT8 *outputBuffer;
	FILE *fp;

	UINT8 *outputBufferPtr;

	//strcpy(inputFileName, argv[1]);
	//strcpy(outputFileName, argv[2]);

	qualityFactor = 1024;
	//imageFormat = GRAY;
	imageFormat = RGB;


	RgbImage srcImage;
	initRgbImage(&srcImage);

  //res = loadRgbImage(inputFileName, &srcImage);
  res = loadRgbImage(&srcImage);

	if (res == 0) {
		printf("Error! Oops: Cannot load the input image!\n");
		return -1;
	}

  //makeGrayscale(&srcImage);

	outputBuffer = (UINT8 *) malloc(OUT_BUFFER_SIZE * sizeof(UINT8));

	outputBufferPtr = outputBuffer;
	outputBufferPtr = encodeImage(
		&srcImage, outputBufferPtr, qualityFactor, imageFormat
	);

	freeRgbImage(&srcImage);

  //fp = fopen(outputFileName, "wb");
  fp = stdout;

	if (fp != NULL) {
		fwrite(outputBuffer, 1, outputBufferPtr - outputBuffer, fp);
		//fclose(fp);
    //fflush(fp);
	}
	free(outputBuffer);
	return 0;
}


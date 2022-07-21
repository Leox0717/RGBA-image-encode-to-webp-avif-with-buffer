// Copyright 2020 Joe Drago. All rights reserved.
// SPDX-License-Identifier: BSD-2-Clause

#include "avif/avif.h"
#include "aom/aom.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char * argv[])
{
    if (argc != 3) {
        fprintf(stderr, "avif_example_encode [encodeYUVDirectly:0/1] [output.avif]\n");
        return 1;
    }
    avifBool encodeYUVDirectly = AVIF_FALSE;
    if (argv[1][0] == '1') {
        encodeYUVDirectly = AVIF_TRUE;
    }
    const char * outputFilename = argv[2];

    int returnCode = 1;
    avifEncoder * encoder = NULL;
    avifRWData avifOutput = AVIF_DATA_EMPTY;
    avifRGBImage rgb;
    memset(&rgb, 0, sizeof(rgb));
    int width = 15360;
    int height = 2160;
    struct timespec time1 = {0, 0}; 
    struct timespec time2 = {0, 0};

    avifImage * image = avifImageCreate(width, height, 8, AVIF_PIXEL_FORMAT_YUV444); // these values dictate what goes into the final AVIF
    // Configure image here: (see avif/avif.h)
    // * colorPrimaries
    // * transferCharacteristics
    // * matrixCoefficients
    // * avifImageSetProfileICC()
    // * avifImageSetMetadataExif()
    // * avifImageSetMetadataXMP()
    // * yuvRange
    // * alphaPremultiplied
    // * transforms (transformFlags, pasp, clap, irot, imir)

    
    // If you have RGB(A) data you want to encode, use this path
    printf("Encoding from converted RGBA\n");
    clock_gettime(CLOCK_REALTIME, &time1);   

    avifRGBImageSetDefaults(&rgb, image);
    rgb.format = AVIF_RGB_FORMAT_BGRA;
    // Override RGB(A)->YUV(A) defaults here: depth, format, chromaUpsampling, ignoreAlpha, alphaPremultiplied, libYUVUsage, etc

    // Alternative: set rgb.pixels and rgb.rowBytes yourself, which should match your chosen rgb.format
    // Be sure to use uint16_t* instead of uint8_t* for rgb.pixels/rgb.rowBytes if (rgb.depth > 8)
    avifRGBImageAllocatePixels(&rgb);
    rgb.rowBytes = width * 4;

    FILE *pd = NULL;
    pd = fopen("rgba15360x2160.out", "rb");
    FILE *opFile;

    rgb.pixels = (uint8_t *) malloc(15360*2160*4);

    printf("malloc finish\n");
    // Fill your RGB(A) data here
    // memset(rgb.pixels, 255, rgb.rowBytes * image->height);
    
    fread(rgb.pixels, sizeof(uint8_t), 15360*2160*4, pd);
    printf("fread finish\n");

    avifResult convertResult = avifImageRGBToYUV(image, &rgb);
    if (convertResult != AVIF_RESULT_OK) {
        fprintf(stderr, "Failed to convert to YUV(A): %s\n", avifResultToString(convertResult));
        goto cleanup;
    }
    printf("RGBToYUV finish\n");


    encoder = avifEncoderCreate();
    // Configure your encoder here (see avif/avif.h):
    // * maxThreads
    // * minQuantizer
    // * maxQuantizer
    // * minQuantizerAlpha
    // * maxQuantizerAlpha
    // * tileRowsLog2
    // * tileColsLog2
    // * speed
    // * keyframeInterval
    // * timescale
    int quality = 0;
    int min_quantizer, max_quantizer, alpha_quantizer;
    max_quantizer = AVIF_QUANTIZER_WORST_QUALITY * ( 100 - quality) / 100;
    min_quantizer = 0;
    alpha_quantizer = 0;

    if ( max_quantizer > 20 ) {
        min_quantizer = max_quantizer - 20;

        if (max_quantizer > 40) {
            alpha_quantizer = max_quantizer - 40;
        }
    }

    encoder->maxThreads = 8;
    encoder->minQuantizer = min_quantizer;
    encoder->maxQuantizer = max_quantizer;
    encoder->minQuantizerAlpha = 0;
    encoder->maxQuantizerAlpha = alpha_quantizer;
    encoder->speed = 8;


    printf("EncoderCreate finish\n");

    // Call avifEncoderAddImage() for each image in your sequence
    // Only set AVIF_ADD_IMAGE_FLAG_SINGLE if you're not encoding a sequence
    // Use avifEncoderAddImageGrid() instead with an array of avifImage* to make a grid image
    avifResult addImageResult = avifEncoderAddImage(encoder, image, 1, AVIF_ADD_IMAGE_FLAG_SINGLE);
    if (addImageResult != AVIF_RESULT_OK) {
        fprintf(stderr, "Failed to add image to encoder: %s\n", avifResultToString(addImageResult));
        goto cleanup;
    }
    printf("AddImage finish\n");

    avifResult finishResult = avifEncoderFinish(encoder, &avifOutput);
    if (finishResult != AVIF_RESULT_OK) {
        fprintf(stderr, "Failed to finish encode: %s\n", avifResultToString(finishResult));
        goto cleanup;
    }

    printf("Encode success: %zu total bytes\n", avifOutput.size);

    clock_gettime(CLOCK_REALTIME, &time2);   
    float temp = (time2.tv_sec - time1.tv_sec)*1000 + (time2.tv_nsec - time1.tv_nsec) / 1000000;
    printf("Compress Time = %f ms\n", temp);

    FILE * f = fopen(outputFilename, "wb");
    size_t bytesWritten = fwrite(avifOutput.data, 1, avifOutput.size, f);
    fclose(f);
    if (bytesWritten != avifOutput.size) {
        fprintf(stderr, "Failed to write %zu bytes\n", avifOutput.size);
        goto cleanup;
    }
    printf("Wrote: %s\n", outputFilename);

    returnCode = 0;
cleanup:
    if (image) {
        avifImageDestroy(image);
    }
    if (encoder) {
        avifEncoderDestroy(encoder);
    }
    avifRWDataFree(&avifOutput);
    avifRGBImageFreePixels(&rgb); // Only use in conjunction with avifRGBImageAllocatePixels()
    return returnCode;
}

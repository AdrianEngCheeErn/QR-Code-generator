#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"  

#include "qrcodegen.h"
#include "qrcodegen.c"

// Save the QR Code as an image (PNG) with no excess background, using red for QR code modules
// Ensures the image has a minimum size of 40x40 pixels
void printQrToImage(const uint8_t qrcode[], const char *filename) {
    int qrSize = qrcodegen_getSize(qrcode);  // Dynamically get the QR code size (number of modules)
    
    // Calculate the minimum image size based on the QR code size and desired module pixel size
    int modulePixelSize = 10;  // Default module pixel size (set each module to 10x10 pixels for visibility)
    
    // Compute the image size based on the QR code size and module pixel size
    int imageSize = qrSize * modulePixelSize;

    // Create a buffer for the image (3 bytes per pixel for RGB)
    unsigned char *image = calloc(imageSize * imageSize * 3, sizeof(unsigned char));
    if (image == NULL) {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    // Fill the image buffer with the QR code (using red pixels for "on" modules, white for "off")
    for (int y = 0; y < qrSize; y++) {
        for (int x = 0; x < qrSize; x++) {
            bool isRed = qrcodegen_getModule(qrcode, x, y);
            int pixelStartX = x * modulePixelSize;
            int pixelStartY = y * modulePixelSize;

            // Fill the area for each module
            for (int dy = 0; dy < modulePixelSize; dy++) {
                for (int dx = 0; dx < modulePixelSize; dx++) {
                    int pixelX = pixelStartX + dx;
                    int pixelY = pixelStartY + dy;

                    if (isRed) {
                        image[(pixelY * imageSize + pixelX) * 3 + 0] = 230;  // Red channel (red)
                        image[(pixelY * imageSize + pixelX) * 3 + 1] = 53;    // Green channel (red)
                        image[(pixelY * imageSize + pixelX) * 3 + 2] = 107;    // Blue channel (red)
                    } else {
                        image[(pixelY * imageSize + pixelX) * 3 + 0] = 255;  // Red channel (white)
                        image[(pixelY * imageSize + pixelX) * 3 + 1] = 255;  // Green channel (white)
                        image[(pixelY * imageSize + pixelX) * 3 + 2] = 255;  // Blue channel (white)
                    }
                }
            }
        }
    }

    // Write the image buffer to a PNG file
    int result = stbi_write_png(filename, imageSize, imageSize, 3, image, imageSize * 3);
    if (result == 0) {
        fprintf(stderr, "Failed to write PNG file: %s\n", filename);
        free(image);
        exit(EXIT_FAILURE);
    } else {
        printf("Successfully saved the PNG file: %s\n", filename);
    }

    free(image);
}

// Function to resize the generated PNG file to 200x200 pixels
void resizePngImage(const char *inputFilename, const char *outputFilename, int newWidth, int newHeight) {
    int width, height, channels;

    // Load the existing image
    unsigned char *inputImage = stbi_load(inputFilename, &width, &height, &channels, 0);
    if (inputImage == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", inputFilename);
        exit(EXIT_FAILURE);
    }

    // Print loaded image info for debugging
    printf("Loaded image: %dx%d, Channels: %d\n", width, height, channels);

    // Allocate memory for the resized image
    unsigned char *resizedImage = malloc(newWidth * newHeight * channels);
    if (resizedImage == NULL) {
        perror("malloc failed");
        stbi_image_free(inputImage);
        exit(EXIT_FAILURE);
    }

    // Perform the resizing
    stbir_resize_uint8_linear(inputImage, width, height, 0, resizedImage, newWidth, newHeight, 0, channels);

    // Save the resized image to a new PNG file
    if (stbi_write_png(outputFilename, newWidth, newHeight, channels, resizedImage, newWidth * channels) == 0) {
        fprintf(stderr, "Failed to write resized PNG file: %s\n", outputFilename);
        free(resizedImage);
        stbi_image_free(inputImage);
        exit(EXIT_FAILURE);
    } else {
        printf("Successfully resized and saved the PNG file: %s\n", outputFilename);
    }

    // Free memory
    free(resizedImage);
    stbi_image_free(inputImage);
}

void doBasicDemo(void) {
    const char *text = "00020101021230600012ucb_pos@ucbp011010000431990226Union Commercial Bank Plc.5204526253038405402125802KH5913INTERNAL TEST6010PHNOM PENH62360703EDC0825004TQTSWZ6cZLb6fJ2wb7OmbB99170013173321453376663043E28";  // User-supplied text
    enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;  // Error correction level

    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

    // Generate the QR code
    bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl,
                                   qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
    if (ok) {
        // Generate the original PNG
        const char *originalPng = "C:/Users/Adrian/Documents/GitHub/QR-Code-generator/c/qrcode.png";
        printQrToImage(qrcode, originalPng);

        // Resize the PNG to 200x200 pixels
        const char *resizedPng = "C:/Users/Adrian/Documents/GitHub/QR-Code-generator/c/qrcode_resized.png";
        resizePngImage(originalPng, resizedPng, 150, 150);
    } else {
        fprintf(stderr, "Failed to generate QR code.\n");
    }
}

int main(void) {
    doBasicDemo();
    return EXIT_SUCCESS;
}

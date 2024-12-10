    /*
 * QR Code generator demo (C)
 * 
 * Run this command-line program with no arguments. The program
 * computes a demonstration QR Codes and print it to the console.
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "qrcodegen.h"
#include "qrcodegen.c"

// The main application program.
int main(void) {
	doBasicDemo();
	return EXIT_SUCCESS;
}



/*---- Utilities ----*/

// Prints the given QR Code to the console.
static void printQr(const uint8_t qrcode[]) {
	int size = qrcodegen_getSize(qrcode);
	int border = 4;
	for (int y = -border; y < size + border; y++) {
		for (int x = -border; x < size + border; x++) {
			fputs((qrcodegen_getModule(qrcode, x, y) ? "##" : "  "), stdout);
		}
		fputs("\n", stdout);
	}
	fputs("\n", stdout);
}


// Save the QR Code as an image (PNG)
void printQrToImage(const uint8_t qrcode[], const char *filename) {
    int size = qrcodegen_getSize(qrcode);  // Get the size of the QR code
    int border = 4;  // Border size in pixels
    int imageSize = (size + 2 * border) * 10;  // Size of the image (QR code + border) * scale factor

    // Create a buffer to hold the pixel data (1 byte per pixel)
    unsigned char *image = calloc(imageSize * imageSize, sizeof(unsigned char));
    if (image == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    // Fill the image buffer with the QR code
    for (int y = 0; y < size + 2 * border; y++) {
        for (int x = 0; x < size + 2 * border; x++) {
            // Compute if the pixel should be black or white
            bool isBlack = (x >= border && x < size + border && y >= border && y < size + border)
                           && qrcodegen_getModule(qrcode, x - border, y - border);
            // Set the pixel value (black = 0, white = 255)
            image[y * imageSize + x] = isBlack ? 0 : 255;
        }
    }

    // Write the image buffer to a PNG file
    int result = stbi_write_png(filename, imageSize, imageSize, 1, image, imageSize);
    if (result == 0) {
        fprintf(stderr, "Failed to write PNG file: %s\n", filename);
        free(image);
        exit(EXIT_FAILURE);
    }

    free(image);
}

// Creates a single QR Code, then prints it to the console.
static void doBasicDemo(void) {
	const char *text = "www.google.com";                // User-supplied text
	enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;  // Error correction level
	
	// Make and print the QR Code symbol
	uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl,
		qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
	if (ok)
	printQrToImage(qrcode, "C:/Users/Adrian/Documents/GitHub/QR-Code-generator/c/qrcode.png");
	printQr(qrcode);
}
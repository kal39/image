#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

// http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html

typedef struct Color {
	unsigned int r;
	unsigned int g;
	unsigned int b;
	
} Color;

typedef struct Image {
	int width;
	int height;
	Color **pixels;
	
} Image;

Image createImage(const int width, const int height, const Color background) {
	Image image;

	image.width = width;
	image.height = height;

	image.pixels = malloc(sizeof(Color*) * width);
	
	for(int i = 0; i < width; i++) {
		image.pixels[i] = malloc(sizeof(Color) * height);
		
		for(int j = 0; j < height; j++)
			image.pixels[i][j] = background;
	}

	return image;
}

void destroyImage(Image *image) {
	free(image->pixels);
}

Image readImage(const char *file) {
	
}

void writeImage(const Image image, const char *file) {
	// headers
	int fileSize = image.width * image.height;
	
	unsigned char fileHeader[14] = {
		'B', 'M', 
		fileSize & 0xFF, fileSize>>8 & 0xFF, fileSize>>16 & 0xFF, fileSize>>24 & 0xFF, 
		0, 0, 0, 0, 
		54, 0, 0, 0
	};

	unsigned char infoHeader[40] = {
		40, 0, 0, 0,
		image.width & 0xFF, image.width>>8 & 0xFF, image.width>>16 & 0xFF, image.width>>24 & 0xFF,
		image.height & 0xFF, image.height>>8 & 0xFF, image.height>>16 & 0xFF, image.height>>24 & 0xFF,
		1, 0,
		24, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};

	// write

	int padLength = image.width - (int)(image.width / 4) * 4;
	unsigned char *pad = malloc(sizeof(unsigned char) * padLength);
	
	FILE *f = fopen(file, "w");

	fwrite(fileHeader, 1, 14, f);
	fwrite(infoHeader, 1, 40, f);

	for(int i = 0; i < image.height; i++) {
		for(int j = 0; j < image.width; j++) {
			fwrite(&image.pixels[j][i].b, 1, 1, f);
			fwrite(&image.pixels[j][i].g, 1, 1, f);
			fwrite(&image.pixels[j][i].r, 1, 1, f);
		}

		fwrite(pad, padLength, 1, f);
	}
}

int main(void) {
	struct timeval start, load, end;

	gettimeofday(&start, NULL);
	
	Image image = createImage(10000, 10000, (Color){0, 255, 0});

	gettimeofday(&load, NULL);
	
	writeImage(image, "test.bmp");

	gettimeofday(&end, NULL);

	printf("loading took %lue-6 [s]\n", (load.tv_sec - start.tv_sec) * 1000000 + load.tv_usec - start.tv_usec);
	printf("drawing took %lue-6 [s]\n", (end.tv_sec - load.tv_sec) * 1000000 + end.tv_usec - load.tv_usec);
}

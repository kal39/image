#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

#define WIDTH 10
#define HEIGHT 10

// http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html

typedef struct Color {
	unsigned int r;
	unsigned int g;
	unsigned int b;
	
} Color;

typedef struct Image {
	int width;
	int height;
	unsigned char *data;
	
} Image;

Image createImage(const int width, const int height, int r, int g, int b) {
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;

	Image image;

	image.width = width;
	image.height = height;

	image.data = malloc(width * height * 3);
	
	for(int i = 0; i < width * height; i++) {
		image.data[i * 3] = b;
		image.data[i * 3 + 1] = g;
		image.data[i * 3 + 2] = r;
	}

	return image;
}

void destroyImage(Image *image) {
	free(image->data);
}

Image readImage(const char *file) {
	Image image;
	FILE *f = fopen(file, "rb");

	// get height and width
	fseek(f, 18, SEEK_SET);
	unsigned char width[4], height[4];

	fread(width, 4, 1, f);
	fread(height, 4, 1, f);
	
	image.width = *(int*)width;
	image.height = *(int*)height;

	image.data = malloc(image.width * image.height * 3);

	// get pixel data
	int padLength = image.width - (int)(image.width / 4) * 4;

	fseek(f, 54, SEEK_SET);

	for(int i = 0; i < image.height; i++) {
		fread(&image.data[i * image.width * 3], image.width * 3, 1, f);
		fread(NULL, padLength, 1, f);
	}

	return image;
	
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
	
	FILE *f = fopen(file, "wb");

	fwrite(fileHeader, 1, 14, f);
	fwrite(infoHeader, 1, 40, f);

	for(int i = 0; i < image.height; i++) {
		fwrite(image.data + image.width * 3 * i, 3, image.width, f);
		fwrite(pad, padLength, 1, f);
	}

	fclose(f);
}

int main(void) {
	struct timeval start, load, draw, destroy, read;

	gettimeofday(&start, NULL);
	
	Image image = createImage(WIDTH, HEIGHT, 0, 255, 0);

	gettimeofday(&load, NULL);
	
	writeImage(image, "test1.bmp");

	gettimeofday(&draw, NULL);

	destroyImage(&image);

	gettimeofday(&destroy, NULL);

	image = readImage("test1.bmp");

	for(int	i = 0; i < image.width * image.height; i++) {
		image.data[i * 3 + 1] -= 100;
	}

	writeImage(image, "test2.bmp");

	gettimeofday(&read, NULL);

	printf("width: %d, height: %d\n", image.width, image.height);
	printf("load:    %lue-6 [s]\n", (load.tv_sec - start.tv_sec) * 1000000 + load.tv_usec - start.tv_usec);
	printf("free:    %lue-6 [s]\n", (draw.tv_sec - load.tv_sec) * 1000000 + draw.tv_usec - load.tv_usec);
	printf("destroy: %lue-6 [s]\n", (destroy.tv_sec - draw.tv_sec) * 1000000 + destroy.tv_usec - draw.tv_usec);
	printf("read:    %lue-6 [s]\n", (read.tv_sec - destroy.tv_sec) * 1000000 + read.tv_usec - destroy.tv_usec);
	
	return 0;
}

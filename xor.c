#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long getfilesize(const char *filename);
void genkey(char *key, long keysize);
void encrypt(const char *filein, const char *fileout, char *key, long keysize);

int main (int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage : source, encrypt, decrypt");
		return EXIT_FAILURE;
	}

	const char *filein = argv[1];
	const char *encryptedfile = argv[2];
	const char *decryptedfile = argv[3];

	long keysize = getfilesize(filein);
	if (keysize == -1) {
		return EXIT_FAILURE;
	}
	char key[keysize];
	genkey(key, keysize);

	encrypt(filein, encryptedfile, key, keysize);
	encrypt(encryptedfile, decryptedfile, key, keysize);

	printf("Done \n");

	return EXIT_SUCCESS;
}


long getfilesize(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		perror("Could not open file\n");
		return -1;
	}

	fseek(file, 0, SEEK_END); //curseur fin du fichier
	long size = ftell(file); //position actuelle (taille fichier)
	fseek(file, 0, SEEK_SET); //curseur debut fichier

	if (ferror(file)) {
		return -1;
	}
	if (fclose(file) != 0) {
		return -1;
	}
	return size;
}

void genkey(char *key, long size) {
	srand((unsigned int)time(NULL)); //init
	for (long i = 0; i < size; i++) {
		key[i] = (char)(rand() % 256);
	}
}

void encrypt(const char *filein, const char *fileout, char *key, long keysize) {
	FILE *fin = fopen(filein, "rb");
	if (fin == NULL) {
		perror("Could not open file(s)");
		return;
	}
	FILE *fout = fopen(fileout, "wb");
	if (fout == NULL) {
		fclose(fin);
		perror("Could not open file(s)");
		return;
	}

	int ch;
	size_t keyindex = 0;
	while ((ch = fgetc(fin)) != EOF) {
		fputc(ch ^ key[keyindex], fout);
		keyindex = (keyindex + 1) % (long unsigned int)keysize; //quand index == size, index = 0
	}

	if (ferror(fin) || ferror(fout)) {
		perror("Error, somewhere...");
	}
	if (fclose(fin) != 0) {
		perror("Error while closing source file");
	}
	if (fclose(fout) != 0) {
		perror("Error while closing destination file");
	}
}

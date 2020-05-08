#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else 
#define PATH_SEPARATOR '/'
#endif 

#define BUFSIZE 256 * 1024
char buf[BUFSIZE];

void readEntireFile(char* path, int verbose) {
	struct stat fs = { 0 };
	if (stat(path, &fs) != 0)
	{
		printf("%s: error while running stat\n", path);
		return;
	}
	long long expectedSize = fs.st_size;

	FILE *file = fopen(path, "rb");
	if (!file) {
		printf("%s: error while opening file\n", path);
		return;
	}

	long long size = 0;
	int n;
	while ((n = fread(buf, 1, BUFSIZE, file)) != 0) {
		size += n;
	}
	if (ferror(file)) {
		perror(path);
	}
	else if (!feof(file)) {
		printf("%s: read 0, but not at EOF yet\n", path);
	} else if (size != expectedSize) {
		printf("%s: read %lld, but expected %lld\n", path, size, expectedSize);
	}

	if (verbose) {
		printf("%s: read %lld bytes\n", path, size);
	}

	fclose(file);
}

void readEntireFileRecursive(char* path, int verbose) {
	if (verbose) {
		printf("%s: reading\n", path);
	}

	struct stat fs = { 0 };
	if (stat(path, &fs) != 0)
	{
		printf("%s: error while running stat\n", path);
		return;
	}

	if (S_ISREG(fs.st_mode)) {
		readEntireFile(path, verbose);
	} else if (S_ISDIR(fs.st_mode)) {
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(path)) != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, "$RECYCLE.BIN") == 0) {
					continue;
				}

				char* tmpPath = malloc(strlen(path) + strlen(ent->d_name) + 2);
				if (path[strlen(path) - 1] != PATH_SEPARATOR) {
					sprintf(tmpPath, "%s%c%s", path, PATH_SEPARATOR, ent->d_name);
				} else {
					sprintf(tmpPath, "%s%s", path, ent->d_name);
				}
				readEntireFileRecursive(tmpPath, verbose);
				free(tmpPath);
			}
			closedir(dir);
		}
		else {
			perror(path);
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2 && argc != 3) {
		printf("Tests that files can be fully read. Traverses directories recursively. Skips \"$RECYCLE.BIN\" and entries that are not either normal files or directories.\n");
		printf("Usage:\n");
		printf("  readallfiles fileOrDirectory [-v]\n");
		printf("    -v  verbose\n");
		printf("Examples:\n");
		printf("  readallfiles f:\\\n");
		printf("  readallfiles f:\\files\n");
		printf("  readallfiles /home/user1\n");
		return 0;
	}
	char* path = argv[1];

	int verbose = 0;
	if (argc == 3) {
		if (strcmp(argv[2], "-v") == 0) {
			verbose = 1;
		}
	}

	readEntireFileRecursive(path, verbose);

	return 0;
}

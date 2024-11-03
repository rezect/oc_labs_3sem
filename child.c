#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define FILESIZE 4096
#define OUTPUT_FILENAME "output.txt"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    perror("Failed to open file");
    return 1;
  }

  char *mapped = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) {
    perror("Failed to map file");
    close(fd);
    return 1;
  }

  FILE *output = fopen(OUTPUT_FILENAME, "w");
  if (output == NULL) {
    perror("Failed to open output file");
    munmap(mapped, FILESIZE);
    close(fd);
    return 1;
  }

  char *line = mapped;
  while (*line != '\0') {
    int num1, num2, num3;
    if (sscanf(line, "%d %d %d", &num1, &num2, &num3) == 3) {
      int sum = num1 + num2 + num3;
      fprintf(output, "%d\n", sum);
    }
    while (*line != '\n' && *line != '\0')
      line++;
    if (*line == '\n')
      line++;
  }

  fclose(output);
  munmap(mapped, FILESIZE);
  close(fd);

  return 0;
}

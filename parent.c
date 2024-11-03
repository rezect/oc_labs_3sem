#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define FILENAME "data.txt"
#define FILESIZE 4096

int main() {
  int fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (fd == -1) {
    perror("Failed to open file");
    return 1;
  }

  if (ftruncate(fd, FILESIZE) == -1) {
    perror("Failed to set file size");
    close(fd);
    return 1;
  }

  char *mapped =
      mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) {
    perror("Failed to map file");
    close(fd);
    return 1;
  }

  char data[FILESIZE];
  printf("Введите чиселки: ");
  fgets(data, FILESIZE, stdin);
  strncpy(mapped, data, strlen(data));

  pid_t pid = fork();
  if (pid < 0) {
    perror("Failed to fork");
    munmap(mapped, FILESIZE);
    close(fd);
    return 1;
  } else if (pid == 0) {
    execlp("./child", "./child", FILENAME, NULL);
    perror("Failed to exec child");
    exit(1);
  } else {
    wait(NULL);
  }

  munmap(mapped, FILESIZE);
  close(fd);

  return 0;
}

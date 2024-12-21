#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include <sys/wait.h>

#define _R_ "\x1b[31m"         // red color for child process output color
#define _G_ "\x1b[32m"         // green color for parent process output color
#define RESET_COLOR "\x1b[0m"  // reset color

int main() {
  char input[1024];

  char file_name[256];
  printf("Введите имя файла: ");
  scanf("%s", &file_name);

  getchar();

  int pipe1[2];
  int pipe2[2];
  pipe(pipe1);
  pipe(pipe2);

  pid_t p = fork();
  if (p == 0) {
    close(pipe1[1]);
    close(pipe2[0]);

    FILE* file = fopen(file_name, "w");
    if (!file) {
      perror("Failed to open file");
      exit(1);
    }

    float sum = 0.0f;
    float num = 0.0f;

    while (read(pipe1[0], &num, sizeof(num))) {
      sum += num;
    }

    fprintf(file, "%f\n", sum);

    write(pipe2[1], &sum, sizeof(sum));

    fclose(file);
    close(pipe1[0]);
    close(pipe2[1]);

    exit(0);
  } else {
    close(pipe1[0]);
    close(pipe2[1]);

    float num = 0.0f;
    float sum = 0.0f;

    printf("Введите числа через пробел: ");
    
    fgets(input, sizeof(input), stdin);
    char* token = strtok(input, " ");
    while (token != NULL) {
      num = strtof(token, NULL);
      write(pipe1[1], &num, sizeof(float));
      token = strtok(NULL, " ");
    }

    close(pipe1[1]);

    read(pipe2[0], &sum, sizeof(sum));
    printf(_G_ "Your sum is: %.2f\n" RESET_COLOR, sum);

    close(pipe2[0]);

    wait(NULL);
  }

  return 0;
}
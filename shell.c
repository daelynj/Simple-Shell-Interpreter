#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0
#define PATH_MAX 4096
#define HOSTNAME_MAX 1024
#define INPUT_SIZE_MAX 128

void print_input_field() {
  char hostname[HOSTNAME_MAX];
  gethostname(hostname, HOSTNAME_MAX);

  char cwd[PATH_MAX];
  char *path = getcwd(cwd, sizeof(cwd));

  char *login = getlogin();

  printf("%s@", login);
  printf("%s  ", hostname);
  printf("%s >", path);
}

void get_user_input(char **user_input) {
  print_input_field();

  char *raw_user_input = readline(" ");

  char *token = strtok(raw_user_input, " ");

  int i;
  for (i = 0; i < INPUT_SIZE_MAX; i++) {
    user_input[i] = token;
    token = strtok(NULL, " ");
  }
}

void execute_user_input(char **user_input) {
  pid_t pid = fork();
  if (pid == 0) {
    // this is in the child process
    int execution_condition = execvp(user_input[0], user_input);
    if (execution_condition == -1) {
      printf("invalid command\n");
      exit(true);
    }
  } else if (pid < 0) {
    printf("forking error");
    exit(true);
  } else {
    // this is in the parent process
    pid_t wait_pid;
    int condition;

    do {
      wait_pid = waitpid(pid, &condition, WUNTRACED);
    } while (!WIFSIGNALED(condition) && !WIFEXITED(condition));
  }
}

int main() {
  while (true) {
    char *user_input[INPUT_SIZE_MAX];
    get_user_input(user_input);
    execute_user_input(user_input);
  }

  return true;
}

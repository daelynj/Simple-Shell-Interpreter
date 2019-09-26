#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define true 1
#define false 0
#define PATH_MAX 4096
#define HOSTNAME_MAX 1024
#define LOGIN_MAX 1024

char *get_hostname() {
  char *hostname = malloc(HOSTNAME_MAX * sizeof(char));
  hostname[HOSTNAME_MAX - 1] = '\0';
  gethostname(hostname, HOSTNAME_MAX);

  return hostname;
}

char *get_path() {
  char cwd[PATH_MAX];
  char *path = malloc(PATH_MAX * sizeof(char));
  path[PATH_MAX - 1] = '\0';
  path = getcwd(cwd, sizeof(cwd));

  return path;
}

char *get_login() {
  char *login = malloc(LOGIN_MAX * sizeof(char));
  login[LOGIN_MAX - 1] = '\0';
  login = getlogin();

  return login;
}

char *build_input_field() {
  char *hostname = get_hostname();
  char *path = get_path();
  char *login = get_login();

  printf("%s", hostname);

  return "hello";
}

char *get_user_input() {
  char *input_field = build_input_field();

  char *user_input = readline(input_field);

  return user_input;
}

int main() {
  char *user_input = get_user_input();
  printf("%s", user_input);

  return 0;
}
